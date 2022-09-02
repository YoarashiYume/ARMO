#ifndef WORKER_H
#define WORKER_H

#include "../Packet.h"

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

    std::list<QByteArray> arrs;

    std::mutex mx, getMx;
    uint32_t countOfThread{}, totalCountOfThread{};

    void updateImage(std::size_t index);
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
    void errorSlot(QAbstractSocket::SocketError sockErr);
    void disconnected();
    void connected();
    void readyRead();
};

#endif // WORKER_H
