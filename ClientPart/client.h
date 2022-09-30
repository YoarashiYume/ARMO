#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QThreadPool>
#include <QImage>
#include <QString>
#include <QQueue>

#include <memory>
#include <mutex>

#include <QRunnable>
#include <functional>

#include "Packet.h"

/**
\brief Class responsible for reading the image and sending it to the Server
*/
class Client final : public QObject
{
    Q_OBJECT
public:
    using port_type = std::uint16_t;

private:
	/**
	\brief Supporting structure for multi-threaded image reading
	Used due to Qt5 where QThreadPool does not have a method to work with std::function
	*/
	struct Runnable : public QRunnable
	{
		std::function<void()> innerFunction; ///< function executing on a thread
		/**
		\brief QRunnable interface
		*/
		void run()
		{
			innerFunction();
		}
	};
	
    std::unique_ptr<QImage> img; ///< Sended image
    std::unique_ptr<QTcpSocket> socket; ///< Socket


    bool isCorrect{true};///< Displays initialization state
    QString errMessage{};///< Initialization error message

    port_type port;///< Port on which the server is running
    QString addr;///< Server address
    std::size_t countOfPacketsSent {128}; ///< Count of packets sent

    std::mutex mx; ///< guarantees the correct filling the message buffer
    QQueue<QByteArray> packetList;///< message buffer

    std::size_t countOfThread; ///< Count of threads to read the image

	/**
	\brief Image reading method
	\param threadId thread index
	*/
    void theadPrepareFunction(std::size_t threadId);
	/**
	\brief Method fills the send buffer
	\param threadId Thread index
	*/
    void addDataToQueue(const QByteArray& data);
	/**
	\brief Method starts the image reading process
	*/
    void prepareData();
	
	/**
	\brief Class initializing method
	\param strHost Server address
	\param nPort Port on which the server is running
	\param countOfPacket Count of packets sent
	\param parent Parent
	*/
    void init(const QString& strHost, port_type nPort,const std::size_t countOfPacket, QObject* parent);

public:
	/**
	\brief Constructor
	\param strHost Server address
	\param nPort Port on which the server is running
	\param countOfPacket Count of packets sent
	\param parent Parent
	*/
    explicit Client(const QString& strHost, port_type nPort,const std::size_t countOfPacket = 128, QObject* parent = nullptr);
	/**
	\brief Constructor
	\param argc Count of input parameters
	\param argv List of input parameters
	\param countOfPacket Count of packets sent
	\param parent Parent
	*/
    explicit Client(int argc, char *argv[],const std::size_t countOfPacket = 128, QObject* parent = nullptr);

    Client() = delete;
    Client(const Client&) = delete;
    Client(Client&&) = delete;
	/**
	\brief Destructor
	*/
    ~Client();
	/**
	\brief Method setss a new image to send, checking for its existence
	\param path Path to image
	\return true if image is image and exists, otherwise false
	*/
    bool setPath(const QString& path);
	/**
	\brief Method return init status
	\return true if the client is properly initiated
	*/
    bool isCorrectStart() const;
	/**
	\brief Method return error message
	\return error message
	*/
    const QString& what() const;

signals:
	/**
	\brief Socket disconnect signal
	*/
    void disconnected();
private slots:

	/**
	\brief Socket disconnect slot
	*/
    void disconnectedSocket();
	/**
	\brief Socket connected slot
	*/
    void connected();
	/**
	\brief Socket is ready to read slot
	*/
    void readyRead();
	/**
	\brief Error slot
	\param sockErr Socket error
	*/
    void slotError (QAbstractSocket::SocketError sockErr);


};

#endif // CLIENT_H
