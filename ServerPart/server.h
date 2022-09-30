#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QString>


#include <unordered_map>
#include <mutex>
#include <vector>
#include <algorithm>
#include <utility>

#include "worker.h"


/**
\brief Class that establishes the connection and starts collecting the image
*/
class Server final : public QTcpServer
{
    Q_OBJECT
private:
    using port_type = std::uint16_t;
    using key_type = std::size_t;
    using value_type = std::unique_ptr<Worker>;
    using image_type = std::unique_ptr<QLabel>;
private:
    port_type port{};///< Port on which the server is running
    QString errMessage{};///< Initialization error message
    bool isCorrect{true};///< Displays initialization state

    std::uint32_t nextId {0};///< Id of the next Worker


    std::mutex mx;///< guarantees the correct filling the Workers buffer

    std::unordered_map<key_type, value_type> storage; ///< guarantees the correct filling the Workers buffer (client handler storage)

    std::unique_ptr<QPushButton> exitButton;///< small GUI to close the server
    std::vector<std::pair<std::size_t, image_type>> imageList; ///< list of built images
	
	/**
	\brief Class initializing method
	\param serverPort Port on which the server is running
	\param parent Parent
	*/
    void init(port_type serverPort, QObject* parent);

public:
	/**
	\brief Class initializing method
	\param serverPort Port on which the server is running
	\param parent Parent
	*/
    explicit  Server(port_type serverPort, QObject* parent = nullptr);
	/**
	\brief Constructor
	\param argc Count of input parameters
	\param argv List of input parameters
	\param parent Parent
	*/
    explicit  Server(int argc, char *argv[], QObject* parent = nullptr);

    Server() = delete;
    Server(const Server&) = delete;
    Server(Server&&) = delete;
	/**
	\brief Destructor
	*/
    ~Server();

	/**
	\brief Method starts the server
	*/
    void startServer();
	/**
	\brief Method return init status
	\return true if the server is properly initiated
	*/
    bool isCorrectStart() const;
	/**
	\brief Method return error message
	\return error message
	*/
    const QString & what() const;

 signals:
	/**
	\brief App closed signal
	*/
    void exit();

protected:
	/**
	\brief Method responsible for connecting a new client
	\param handle socket handler
	*/
	void incomingConnection(qintptr handle) override;

protected slots:
	/**
	\brief Client disconnected slot
	*/
    void disconnected();
	/**
	\brief App closed slot
	*/
    void quit();


};

#endif // SERVER_H
