#include <QApplication>
#include <QDebug>

#include "clientserverapp.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClientServerApp app(argc, argv, &a);
    if (!app.isCorrectStart())
    {

        qDebug() << app.what();
        qDebug() << "Server usage : <progname> <port> -s";
        qDebug() << "Client usage : <progname> <imagePath> <serverAddr> <port> -c";
        return 1;
    }
    return a.exec();
}
