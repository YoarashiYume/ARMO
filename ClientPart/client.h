#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QThreadPool>
#include <QImage>
#include <QString>
#include <QQueue>

#include <memory>
#include <mutex>

#include <QRunnable>
#include <functional>

struct Runnable : public QRunnable
{
    std::function<void()> innetFunction;
    void run()
    {
        innetFunction();
    }
};

#include "Packet.h"

class Client final : public QObject
{
    Q_OBJECT
public:
    using port_type = std::uint16_t;

private:
    std::unique_ptr<QImage> img;
    std::unique_ptr<QTcpSocket> socket;


    bool isCorrect{true};
    QString errMessage{};

    port_type port;
    QString addr;
    std::size_t countOfPacketsSent {128};

    std::mutex mx;
    QQueue<QByteArray> packetList;

    std::size_t countOfThread;

    void theadPrepareFunction(std::size_t threadId);
    void addDataToQueue(const QByteArray& data);
    void prepareData();

    void init(const QString& strHost, port_type nPort,const std::size_t countOfPacket, QObject* parent);

public:
    explicit Client(const QString& strHost, port_type nPort,const std::size_t countOfPacket = 128, QObject* parent = nullptr);
    explicit Client(int argc, char *argv[],const std::size_t countOfPacket = 128, QObject* parent = nullptr);

    Client() = delete;
    Client(const Client&) = delete;
    Client(Client&&) = delete;

    bool setPath(const QString& path);

    bool isCorrectStart() const;
    const QString& what() const;

signals:
    void disconnected();
private slots:

    void disconnectedSocket();
    void connected();
    void readyRead();
    void slotError (QAbstractSocket::SocketError sockErr);


};

#endif // CLIENT_H
