#include <QApplication>
#include <iostream>

#include "server.h"
QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server app(argc, argv, &a);
    if (!app.isCorrectStart())
    {
        qStdOut() << app.what();
        qStdOut() << "Server usage : <progname> <port>";
        return 1;
    }
    app.startServer();
    return a.exec();
}

