#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QThreadPool>
#include <QImage>
#include <QString>
#include <QQueue>

#include <memory>

#include "../ServerPart/Packet.h"

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
    std::atomic_uint64_t countOfPixelInPackage {8};

    std::mutex mx;
    QQueue<QByteArray> packetList;

    std::size_t countOfThread;

    void theadSendFunction(std::size_t threadId);   
    void addDataToQueue(const QByteArray& data);
    void startSend();

    void init(const QString& strHost, port_type nPort, QObject* parent);

public:
    explicit Client(const QString& strHost, port_type nPort, QObject* parent = nullptr);
    explicit Client(int argc, char *argv[], QObject* parent = nullptr);

    Client() = delete;
    Client(const Client&) = delete;
    Client(Client&&) = delete;

    bool setPath(const QString& path);
    void setCountOfPixelInPackage(const std::size_t count);

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
