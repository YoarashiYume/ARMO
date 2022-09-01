#include "client.h"


void Client::init(const QString& strHost,port_type nPort, QObject* parent)
{
    this->port = nPort;
    this->addr = strHost;
    this->socket.reset(new QTcpSocket{this});

    //Configure socket and threadpool
    connect(this, SIGNAL(disconnected()), parent, SLOT(quit()));
    connect(socket.get(), SIGNAL(errorOccurred(QAbstractSocket::SocketError)),
            this, SLOT(slotError(QAbstractSocket::SocketError)));
    connect(socket.get(), SIGNAL(connected()),
            this, SLOT(connected()));
    connect(socket.get(), SIGNAL(disconnected()),
            this, SLOT(disconnectedSocket()));
    connect(socket.get(), SIGNAL(readyRead()),
            this, SLOT(readyRead()));
    socket->connectToHost(addr, port);

    this->countOfThread = std::thread::hardware_concurrency();
    QThreadPool::globalInstance()->setMaxThreadCount(countOfThread);
}


Client::Client(const QString& strHost, port_type nPort, QObject* parent )
    :QObject(parent)
{
    this->init(strHost, nPort, parent);
}

Client::Client(int argc, char *argv[], QObject* parent)
    :QObject(parent) , socket{new QTcpSocket{this}}
{
    // Parsing input arguments
    if (argc !=  4)
    {
        this->errMessage =  "Missing arguments\n";
        this->isCorrect = false;
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
        this->init(strHost, port, parent);
        this->isCorrect &= this->setPath(path);
    }

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

void Client::setCountOfPixelInPackage(const std::size_t count)
{
    this->countOfPixelInPackage.store(count);
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
void Client::startSend()
{
    //Create tasks
    for (auto i = 0u; i < countOfThread; ++i)
        QThreadPool::globalInstance()->start(std::bind(&Client::theadSendFunction, this, i));

    //Starts to send a packets
    while (QThreadPool::globalInstance()->activeThreadCount() || this->packetList.size())
    {
        mx.lock();
        if (this->packetList.empty())
        {
            mx.unlock();
            continue;
        }

        auto data = std::move(packetList.front());
        packetList.pop_front();
        mx.unlock();


        socket->write(data);
        socket->flush();
    }
    socket->close();
}
void Client::addDataToQueue(const QByteArray& data)
{
    //Adds buffer data to queue
    std::lock_guard<decltype(mx)> lg{mx};
    this->packetList.emplace_back(data);
}
void Client::theadSendFunction(std::size_t threadId)
{
    //Prepare data before reading
    auto size = this->img->size();

    Packet pac{};
    uint8_t currentPacketCount = 0;

    QByteArray info{static_cast<qsizetype>(countOfPixelInPackage*sizeof(Packet)), 0}; // Configure buffer size


    for (auto x = threadId; x < size.width(); x+=countOfThread)
        for (auto y = 0; y < size.height(); ++y, ++currentPacketCount )
        {
            if (currentPacketCount == countOfPixelInPackage)
            {
                //Sending the buffer to the send queue
                currentPacketCount = 0;
                addDataToQueue(info);
            }
            //Fill buffer
            pac.xCoord = x;
            pac.yCoord = y;
            pac.rgba = this->img->pixel(x,y);
            memcpy(info.data() +currentPacketCount * sizeof(pac) , &pac, sizeof(pac));

        }
    //last buffer send to the send queue
    if (currentPacketCount != 0)
    {
        info.resize(currentPacketCount * sizeof(pac));
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
    socket->write(info);
    socket->flush();
    socket->waitForBytesWritten(1000);

    startSend();
}
void Client::readyRead()
{
    startSend();
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
