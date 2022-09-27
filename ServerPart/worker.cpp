#include "worker.h"

Worker::Worker(const qintptr& handle, QObject * parent)
    :QObject(parent), ptr(socket_type(new QTcpSocket(parent)))
{
    //Configure socket
    this->ptr->setSocketDescriptor(handle);

    connect(ptr.get(), SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(ptr.get(), SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(ptr.get(), SIGNAL(connected()), this, SLOT(connected()));
    connect(ptr.get(), SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(errorSlot(QAbstractSocket::SocketError)));


}
Worker::Worker(Worker&& other)
{
    if(this!= &other)
    {
        this->ptr.swap(other.ptr);
        this->image.swap(other.image);
        other.ptr.reset();
        other.image.reset();
    }
}

bool Worker::isFinished() const
{
    return isDone.load();
}


void Worker::updateImage(std::size_t index)
{
    //Updating pixels in image
    auto & info = *std::next(arrs.begin(),index);
    Packet pac{};

    for (auto i = 0u; i < info.size(); i+=sizeof(Packet))
    {
        memcpy(&pac, info.data() + i, sizeof(Packet));
        this->image->setPixel(pac.xCoord, pac.yCoord, pac.rgba);
    }
    info.clear();
    mx.lock();
    countOfThread++;
    mx.unlock();
}
void Worker::connected()
{
}


void Worker::readyRead()
{

    if (image == nullptr)
    {
        auto socketInfo = ptr->read(sizeof(InitialPacket));
        //Setting the  data (width and height) for the image
        InitialPacket packet;

        memcpy(&packet, socketInfo.data(), sizeof(InitialPacket));
        this->image.reset(new QImage{ packet.width, packet.height, QImage::Format::Format_ARGB32});
    }
    else
    {
        //Reading data and transferring it to an image
        std::lock_guard<decltype(getMx)> lg{getMx};
        auto socketInfo = ptr->read(sizeof(QByteArray::size_type));
        QByteArray::size_type size;

        memcpy(&size, socketInfo.data(), sizeof(QByteArray::size_type));

        socketInfo = ptr->read(size);
        while (socketInfo.size() != size)
        {
            //Reading all buffer
            if (!ptr->waitForReadyRead())
            {
                errorSlot(QAbstractSocket::SocketError::SocketTimeoutError);
                return;
            }
            auto needToRead = size - socketInfo.size();
            socketInfo.append(ptr->read(needToRead));
        }
        arrs.emplace_back(socketInfo);
        Runnable* runnableObj = new Runnable;
        runnableObj->innetFunction = std::bind(&Worker::updateImage, this, arrs.size()-1);
        runnableObj->setAutoDelete(true);
        QThreadPool::globalInstance()->start(runnableObj);
        ++totalCountOfThread;
    }
    ptr->write("done");
    ptr->waitForBytesWritten();
    ptr->flush();
}

void Worker::errorSlot(QAbstractSocket::SocketError sockErr)
{
    if ( sockErr != QAbstractSocket::RemoteHostClosedError)
    {
        QString strError =
                "Error: " + (sockErr == QAbstractSocket::HostNotFoundError ?
                                 "The host was not found." :
                                 sockErr == QAbstractSocket::ConnectionRefusedError ?
                                     "The connection was refused." :
                                     QString(ptr->errorString()));
        qDebug() << strError;
    }
    ptr->close();
}


void Worker::disconnected()
{
    while (true)
    {
        QThread::msleep(10);
        std::lock_guard<decltype(mx)> lg{mx};
        if (countOfThread == totalCountOfThread)
            break;

    }

    isDone.store(true);
    ptr->close();
    emit socketDisconnected();
}

Worker::image_type Worker::getImage()
{
    return image;
}
