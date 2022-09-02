#include "clientserverapp.h"

ClientServerApp::ClientServerApp(int argc, char *argv[], QObject *parent)
{
    if (argc != 3 && argc != 5)
    {
        this->errMessage = "Incorrect count of arguments\n";
        this->isCorrect = false;
    }
    else
    {
        QString type{argv[argc-1]};
        if (type == "-c")
        {
            this->currentType = Type::CLIENT;
            cApp.reset(new Client(argc-1, argv,128, parent));
            this->isCorrect = cApp->isCorrectStart();
        }
        else if (type == "-s")
        {
            this->currentType = Type::SERVER;
            sApp.reset(new Server(argc-1, argv, parent));
            this->isCorrect = sApp->isCorrectStart();
            if (isCorrect)
                sApp->startServer();
        }
        else
        {
            this->errMessage = "Incorrect type\n";
            this->isCorrect = false;
        }
    }
}

bool ClientServerApp::isCorrectStart() const
{
    return this->isCorrect;
}
const QString ClientServerApp::what() const
{
    QString err;
    switch (this->currentType) {
    case Type::CLIENT:
        err = cApp->what();
        break;
    case Type::SERVER:
        err = sApp->what();
        break;
    default:
        break;
    }
    return this->errMessage.size() ? this->errMessage : err;
}
