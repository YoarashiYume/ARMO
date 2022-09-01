#include <QApplication>
#include <iostream>

#include "clientserverapp.h"

QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClientServerApp app(argc, argv, &a);
    if (!app.isCorrectStart())
    {
        qStdOut() << app.what();
        qStdOut() << "Server usage : <progname> <port> -s\n";
        qStdOut() << "Client usage : <progname> <imagePath> <serverAddr> <port> -c\n";
        return 1;
    }
    return a.exec();
}
