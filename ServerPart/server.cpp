#include "server.h"

void Server::init(port_type serverPort, QObject* parent)
{
    this->port = serverPort;
    //Configure threadpool and exit button
    QThreadPool::globalInstance()->setMaxThreadCount(QThread::idealThreadCount());

    this->exitButton.reset(new QPushButton{});
    this->exitButton->setText("Exit");
    this->exitButton->show();

    connect(this->exitButton.get(), SIGNAL(clicked()), this, SLOT(quit()));
    connect(this, SIGNAL(exit()), parent, SLOT(quit()));

}

Server::Server(int argc, char *argv[], QObject* parent)
    : QTcpServer(parent)
{
    // Parsing input arguments
    if (argc !=  2)
    {
        errMessage = "Missing port\n";
        this->isCorrect = false;
    }

    QString strPort{argv[1]};
    bool isOk;
    this->port = strPort.toUShort(&isOk);
    if (isOk == false)
    {
        errMessage += "Incorrect port value";
        this->isCorrect = false;
    }
    if(this->isCorrect)
        this->init(port, parent);
}

Server::Server(port_type serverPort, QObject* parent)
    : QTcpServer(parent)
{
    this->init(serverPort, parent);
}

void Server::startServer()
{
    this->isCorrect = listen(QHostAddress::Any, port);
}

const QString & Server::what() const
{
    return this->errMessage;
}

bool Server::isCorrectStart() const
{
    return this->isCorrect;
}


void Server::incomingConnection(qintptr handle)
{
    //Create a new handler for each new client
    auto ptr = value_type{new Worker{ handle, this}};
    connect(ptr.get(), SIGNAL(socketDisconnected()), this, SLOT(disconnected()));

    std::lock_guard<decltype(mx)> lg{mx};
    storage.emplace(nextId, std::move(ptr));
    ++nextId;
}
void Server::quit()
{
    if(isListening())
        this->close();
    emit exit();
}
Server::~Server()
{
    if(isListening())
        this->close();
}


void Server::disconnected()
{
    //When any socket is disconnected (full image delivery is expected), the image is displayed

    std::lock_guard<decltype(mx)> lg{mx};
    auto work = std::find_if(storage.begin(), storage.end(), [](typename decltype(storage)::value_type & el)
    {
            return el.second->isFinished();
    });

    auto closedImage = std::remove_if(imageList.begin(), imageList.end(), [this](typename decltype(imageList)::value_type& el)
    {
        if (el.second->isHidden())
            this->storage.erase(el.first);//Handler destruction
        return el.second->isHidden();
    });

    imageList.erase(closedImage, imageList.end());

    this->imageList.emplace_back(work->first, nullptr);

    this->imageList.back().second->setPixmap(QPixmap::fromImage(*work->second->getImage()));

    //Show image in foreground
    Qt::WindowFlags eFlags = this->imageList.back().second->windowFlags();
    eFlags |= Qt::WindowStaysOnTopHint;
    this->imageList.back().second->setWindowFlags(eFlags);
    this->imageList.back().second->resize(work->second->getImage()->size());
    this->imageList.back().second->show();


}
