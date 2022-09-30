#include "client.h"


void Client::init(const QString& strHost,port_type nPort,const std::size_t countOfPacket, QObject* parent)
{
    this->port = nPort;
    this->addr = strHost;
    this->countOfPacketsSent = countOfPacket;
    this->socket.reset(new QTcpSocket{this});

    //Configure socket and threadpool
    connect(this, SIGNAL(disconnected()), parent, SLOT(quit()));
    connect(socket.get(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotError(QAbstractSocket::SocketError)));
    connect(socket.get(), SIGNAL(connected()), this, SLOT(connected()));
    connect(socket.get(), SIGNAL(disconnected()), this, SLOT(disconnectedSocket()));
    connect(socket.get(), SIGNAL(readyRead()), this, SLOT(readyRead()));
    socket->connectToHost(addr, port);

    this->countOfThread = 2*QThread::idealThreadCount()-1;
    QThreadPool::globalInstance()->setMaxThreadCount(countOfThread);
}


Client::Client(const QString& strHost, port_type nPort,const std::size_t countOfPacket, QObject* parent )
    :QObject(parent)
{
    this->init(strHost, nPort,countOfPacket, parent);
}

Client::Client(int argc, char *argv[],const std::size_t countOfPacket, QObject* parent)
    :QObject(parent) , socket{new QTcpSocket{this}}
{
    // Parsing input arguments
    if (argc !=  4)
    {
        this->errMessage =  "Missing arguments\n";
        this->isCorrect = false;
        return;
    }
    QString path{argv[1]},strHost{argv[2]}, strPort{argv[3]};

    bool isOk;
    port_type port = strPort.toUShort(&isOk);
    if (isOk == false)
    {
        this->errMessage += "Incorrect port value\n";
        this->isCorrect = false;
    }
    if(this->isCorrect)
    {
        this->init(strHost, port,countOfPacket, parent);
        this->isCorrect &= this->setPath(path);
    }

}

Client::~Client()
{
    socket->disconnect();
}

bool Client::isCorrectStart() const
{
    return this->isCorrect;
}
const QString& Client::what() const
{
    return this->errMessage;
}

void Client::disconnectedSocket()
{
    emit disconnected();
}


bool Client::setPath(const QString& path)
{
    //Validates the input path
    this->img.reset( new QImage{path});
    auto result = true;
    if(img->isNull())
    {
        result = false;
        this->errMessage += "The selected file does not exist or is not an image\n";
    }
    else
    {
        //Validates the image size
        auto size = this->img->size();
        auto maxSize = std::numeric_limits<uint16_t>::max();
        if (size.height() > maxSize || size.width() > maxSize)
        {
            result = false;
            this->errMessage += "Image dimensions must be less than 65536 x 65536";
        }
    }
    return result;
}
void Client::prepareData()
{
    //Create tasks
    for (auto i = 0u; i < countOfThread; ++i)
    {
        Client::Runnable *runnableObj = new Client::Runnable;
        runnableObj->innerFunction = std::bind(&Client::theadPrepareFunction, this, i);
        runnableObj->setAutoDelete(true);
        QThreadPool::globalInstance()->start(runnableObj);
    }
    QThreadPool::globalInstance()->waitForDone();
}
void Client::addDataToQueue(const QByteArray& data)
{
    //Adds buffer data to queue
    std::lock_guard<decltype(mx)> lg{mx};
    this->packetList.enqueue(data);
}
void Client::theadPrepareFunction(std::size_t threadId)
{
    //Prepare data before reading
    auto size = this->img->size();

    Packet pac{};
    uint32_t currentPacketSize = 0;

    auto currentSize = countOfPacketsSent * sizeof(Packet);

    QByteArray info{static_cast<QByteArray::size_type>(currentSize), 0}; // Configure buffer size


    for (auto x = threadId; x < size.width(); x+=countOfThread)
        for (auto y = 0; y < size.height(); ++y, currentPacketSize+=sizeof(Packet))
        {
            if (currentSize == currentPacketSize)
            {
                //Sending the buffer to the send queue
                currentPacketSize = 0;
                addDataToQueue(info);
            }
            //Fill buffer
            pac.xCoord = static_cast<std::uint16_t>(x);
            pac.yCoord = static_cast<std::uint16_t>(y);
            pac.rgba = this->img->pixel(x,y);
            memcpy(info.data() + currentPacketSize , &pac, sizeof(Packet));

        }
    //Sends the last buffer to the send queue
    if (currentPacketSize != 0)
    {
        info.resize(currentPacketSize);
        addDataToQueue(info);
    }
}


void Client::connected()
{
    //Sends initial data of the image
    auto size = this->img->size();

    InitialPacket initPacket{static_cast<uint16_t>(size.height()), static_cast<uint16_t>(size.width())};
    QByteArray info{sizeof(initPacket), 0};

    memcpy(info.data(), &initPacket, sizeof(initPacket));
    prepareData();
    socket->write(info);
    socket->flush();

}
void Client::readyRead()
{
    //Sends a packets
    auto answer = socket->readAll();
    if (this->packetList.empty())
    {
        socket->close();
        emit disconnected();
    }
    else
    {
        QByteArray temp{sizeof(QByteArray::size_type), 0};
        QByteArray::size_type msgSize = packetList.front().size();
        memcpy(temp.data(), &msgSize, sizeof(QByteArray::size_type));
        socket->write(temp,temp.size());

        auto realWrite = socket->write(packetList.front(),packetList.front().size());
        while (realWrite != msgSize)
            realWrite += socket->write(packetList.front().data()+realWrite,packetList.front().size()-realWrite);

        socket->waitForBytesWritten();
        socket->flush();
        packetList.pop_front();
    }
}
void Client::slotError(QAbstractSocket::SocketError sockErr)
{
    QString strError =
            "Error: " + (sockErr == QAbstractSocket::HostNotFoundError ?
                             "The host was not found." :
                             sockErr == QAbstractSocket::RemoteHostClosedError ?
                                 "The remote host is closed." :
                                 sockErr == QAbstractSocket::ConnectionRefusedError ?
                                     "The connection was refused." :
                                     QString(socket->errorString()));
    qDebug() << strError;
    emit disconnected();
}
