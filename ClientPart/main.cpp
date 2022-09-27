#include <QCoreApplication>
#include <iostream>

#include "client.h"
QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Client client{argc, argv,128, &a};

    if (!client.isCorrectStart())
    {
        qDebug() << "usage : <progname> <imagePath> <serverAddr> <port>";
        qDebug() << client.what();
        return 1;
    }

    return a.exec();
}

