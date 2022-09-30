#ifndef WORKER_H
#define WORKER_H

#include "Packet.h"

#include <QTcpSocket>
#include <QThreadPool>
#include <QImage>

#include <memory>
#include <atomic>
#include <mutex>
#include <QRunnable>
#include <functional>


/**
	\brief Сlass that reads information from a socket and collects an image based on it
	*/
class Worker final: public QObject
{
    Q_OBJECT
public:
    using image_type = std::shared_ptr<QImage>;
private:
    using socket_type = std::unique_ptr<QTcpSocket>;
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

private:
    image_type image; ///< Received image
    socket_type ptr;	///< Socket
    std::atomic_bool isDone;///< Сurrent state of image assembling

    std::list<QByteArray> arrs;///< Received image information. Cleared during build process.

    std::mutex mx, ///< guarantees the correct change of variable countOfThread
		getMx;///< guarantees the correct filling and reading of the message buffer
    uint32_t countOfThread{}, ///< Count of threads that have finished executing
	totalCountOfThread{};///< Total amount of thread

	/**
	\brief Method responsible for transferring data from the buffer to the image
	\param index buffer index in Worker::arrs
	*/
    void updateImage(std::size_t index);
public:
    Worker() = delete;
	/**
	\brief Constructor
	\param handle socket handler
	\param parent parent
	*/
    Worker(const qintptr& handle, QObject * parent = nullptr);
    Worker(const Worker&) = delete;
	/**
	\brief Move constructor
	*/
    Worker(Worker&&);

	/**
	\brief Method returns the current state of the build
	\return the current state of the build
	*/
    bool isFinished() const;
	/**
	\brief Method returns the received image
	\return Received image
	*/
    image_type getImage() ;

signals:
	/**
	\brief Socket disconnect signal
	*/
    void socketDisconnected();

public slots:
	/**
	\brief Error slot
	\param sockErr Socket error
	*/
    void errorSlot(QAbstractSocket::SocketError sockErr);
	/**
	\brief Socket disconnect slot
	*/
    void disconnected();
	/**
	\brief Socket connected slot
	*/
    void connected();
	/**
	\brief Socket is ready to read slot
	*/
    void readyRead();
};

#endif // WORKER_H
