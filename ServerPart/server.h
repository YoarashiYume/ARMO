#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QString>


#include <unordered_map>
#include <mutex>
#include <thread>
#include <vector>


#include "worker.h"



class Server final : public QTcpServer
{
    Q_OBJECT
private:
    using port_type = std::uint16_t;
    using key_type = std::size_t;
    using value_type = std::unique_ptr<Worker>;
    using image_type = std::unique_ptr<QLabel>;
private:
    port_type port{};
    QString errMessage{};
    bool isCorrect{true};

    std::uint32_t nextId {0};


    std::mutex mx;

    std::unordered_map<key_type, value_type> storage; //client handler storage

    std::unique_ptr<QPushButton> exitButton;
    std::vector<std::pair<std::size_t, image_type>> imageList;

    void init(port_type serverPort, QObject* parent);

public:
    explicit  Server(port_type serverPort, QObject* parent = nullptr);
    explicit  Server(int argc, char *argv[], QObject* parent = nullptr);

    Server() = delete;
    Server(const Server&) = delete;
    Server(Server&&) = delete;

    ~Server();

    void startServer();

    bool isCorrectStart() const;
    const QString & what() const; //return erroe string

 signals:
    void exit();

protected:
     void incomingConnection(qintptr handle) override;

protected slots:
    void disconnected();
    void quit();


};

#endif // SERVER_H
