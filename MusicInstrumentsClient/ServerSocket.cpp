#include "ServerSocket.h"

ServerSocket::ServerSocket(quint16 port, const QHostAddress& serverAddress, quint16 serverPort, QObject* parent)
	: QObject(parent), mPort(port), mServerAddress(serverAddress), mServerPort(serverPort)
{
	mSocket = std::make_unique<QUdpSocket>(this);
	mSocket->bind(QHostAddress::Any, port);
	connect(mSocket.get(), &QUdpSocket::readyRead, this, &ServerSocket::getData);
}

ServerSocket::~ServerSocket()
{
	quit();
}

void ServerSocket::establishConnection()
{
	QByteArray data;
	QDataStream command(&data, QIODevice::WriteOnly);
	command << static_cast<quint8>(Commands::EstablishConnection);
	mSocket->writeDatagram(data, mServerAddress, mServerPort);
}

void ServerSocket::sendShortMsg(quint32 msg)
{
	QByteArray data;
	QDataStream command(&data, QIODevice::WriteOnly);
	command << static_cast<quint8>(Commands::ShortMsg)
			<< msg;
	mSocket->writeDatagram(data, mServerAddress, mServerPort);
}

void ServerSocket::quit()
{
	QByteArray data;
	QDataStream command(&data, QIODevice::WriteOnly);
	command << static_cast<quint8>(Commands::Quit);
	mSocket->writeDatagram(data, mServerAddress, mServerPort);
}

void ServerSocket::getData()
{
	QByteArray bytes(mSocket->pendingDatagramSize(), '\0');
	mSocket->readDatagram(bytes.data(), mSocket->pendingDatagramSize());
	emit dataReceived(std::move(bytes));
}
