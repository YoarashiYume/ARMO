#ifndef CLIENTSERVERAPP_H
#define CLIENTSERVERAPP_H

#include <QObject>

#include "ServerPart/server.h"
#include "ClientPart/client.h"

#include <variant>



class ClientServerApp final
{
private:

    enum Type : uint16_t
    {
      SERVER = 0,
        CLIENT = 1
    };
    using server_type = std::unique_ptr<Server>;
    using client_type = std::unique_ptr<Client>;

    server_type sApp;
    client_type cApp;

    bool isCorrect{true};
    QString errMessage{};
    Type currentType;
public:


    ClientServerApp(int argc, char *argv[], QObject *parent = nullptr);
    ~ClientServerApp() = default;

    void setCountOfPixelInPackage(const std::size_t count);

    bool isCorrectStart() const;
    const QString what() const;
};

#endif // CLIENTSERVERAPP_H
