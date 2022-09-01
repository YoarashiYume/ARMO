#include "worker.h"

Worker::Worker(const qintptr& handle, QObject * parent)
    :QObject(parent), ptr(socket_type(new QTcpSocket(parent)))
{
    //COnfigure socket
    this->ptr->setSocketDescriptor(handle);

    connect(ptr.get(), SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(ptr.get(), SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(ptr.get(), SIGNAL(connected()), this, SLOT(connected()));


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


void Worker::updateImage(QByteArray info)
{
    //Updating pixels in image
    Packet pac;
    for (auto i = 0u; i < info.size(); i+=sizeof(Packet))
    {
        memcpy(&pac, info.data() + i, sizeof(Packet));
        this->image->setPixelColor(pac.xCoord, pac.yCoord, pac.rgba);
    }
}
void Worker::connected()
{
}

void Worker::readyRead()
{
    auto socketInfo = ptr->readAll();
    if (image == nullptr)
    {
        //Setting the  data (width and height) for the image
        InitialPacket packet;

        memcpy(&packet, socketInfo.data(), sizeof(InitialPacket));
        this->image.reset(new QImage{ packet.width, packet.height, QImage::Format::Format_ARGB32});
    }
    else
        QThreadPool::globalInstance()->start(std::bind(std::bind(&Worker::updateImage, this, socketInfo)));
}

void Worker::disconnected()
{
    isDone.store(true);
    ptr->close();
    emit socketDisconnected();
}

Worker::image_type Worker::getImage()
{
    return image;
}
