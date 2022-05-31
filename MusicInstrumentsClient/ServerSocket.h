#pragma once
#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <memory>
#include <QByteArray>
#include "Windows.h"
#include "mmeapi.h"

class ServerSocket : public QObject
{
	Q_OBJECT

public:

	enum class Commands
	{
		CreateRoom,
		EstablishConnection,
		ShortMsg,
		Quit,
	};

	ServerSocket(quint16 port, const QHostAddress& serverAddress, quint16 serverPort, QObject* parent = nullptr);

	~ServerSocket();

	void setServerPort(quint16 port) { mServerPort = port; }

	void createRoom();

	void establishConnection(quint8 instrument);

	void sendShortMsg(quint32 msg);

	void quit();

signals:
	void dataReceived(QByteArray data);

private:

	void getData();

	QHostAddress mServerAddress;
	quint16 mServerPort;

	// Port which receiver listens to
	quint16 mPort;
	std::unique_ptr<QUdpSocket> mSocket;
};

