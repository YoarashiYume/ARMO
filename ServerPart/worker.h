#ifndef WORKER_H
#define WORKER_H

#include "Packet.h"

#include <QTcpSocket>
#include <QThreadPool>
#include <QImage>

#include <memory>


class Worker final: public QObject
{
    Q_OBJECT
public:
    using image_type = std::shared_ptr<QImage>;
private:
    using socket_type = std::unique_ptr<QTcpSocket>;

private:
    image_type image;
    socket_type ptr;
    std::atomic_bool isDone;

    void updateImage(QByteArray info);  
public:
    Worker() = delete;
    Worker(const qintptr& handle, QObject * parent = nullptr);
    Worker(const Worker&) = delete;
    Worker(Worker&&);

    ~Worker() = default;

    bool isFinished() const;

    image_type getImage() ;

signals:
    void socketDisconnected();

public slots:
    void disconnected();
    void connected();
    void readyRead();
};

#endif // WORKER_H
