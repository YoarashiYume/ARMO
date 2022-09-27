#include <QCoreApplication>
#include <iostream>

#include "server.h"
QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server app(argc, argv, &a);
    if (!app.isCorrectStart())
    {
        qStdOut() << app.what();
        qStdOut() << "Server usage : <progname> <port> -s\n";
        qStdOut() << "Client usage : <progname> <imagePath> <serverAddr> <port> -c\n";
        return 1;
    }
    return a.exec();
}

