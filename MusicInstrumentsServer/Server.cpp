#include "Server.h"

#include <QByteArray>
#include <QDataStream>
#include <QNetworkInterface>
#include <qdebug.h>

QHostAddress Server::getIpAddress()
{
    const QHostAddress& localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress& address : QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
            return address;
    }
    return QHostAddress::LocalHost;
}

Server::Server(quint16 port, QWidget *parent)
    : QMainWindow(parent), mPort(port)
{
    for (int i = 0; i < 10; ++i)
        mAvailableChannels.push(i);

    ui.setupUi(this);

    // TODO менять бинд при изменении пользователем адреса
    mReceiver.bind(mHostAddress, mPort);
    connect(&mReceiver, &QUdpSocket::readyRead, this, &Server::receiveData);

    ui.teLog->append("Waiting for connections on IP: " + mHostAddress.toString() + ":" + QString::number(mPort));
}

void Server::receiveData()
{
    QHostAddress address;
    quint16 port;
    QByteArray data(mReceiver.pendingDatagramSize(), '\0');
    mReceiver.readDatagram(data.data(), mReceiver.pendingDatagramSize(), &address, &port);
    
    QByteArray datagram;
    QDataStream in(&data, QIODevice::ReadOnly), out(&datagram, QIODevice::WriteOnly);
    quint8 command;
    quint32 message;
    ClientAddress client;
    QMap<ClientAddress, quint8>::const_iterator it;
    while (!in.atEnd()) {
        in >> command;
        switch (command)  
        {
        case Commands::EstablishConnection:
            if (mAvailableChannels.empty())
                return;
            mClients[ClientAddress(address, port)] = mAvailableChannels.top();
            mAvailableChannels.pop();
            ui.teLog->append("Connected: " + QHostAddress(address).toString() + ":" + QString::number(port));
            break;
        case Commands::Quit:
            client = ClientAddress(address, port);
            mAvailableChannels.push(mClients[client]);
            mClients.remove(client);
            ui.teLog->append("Disconnected: " + QHostAddress(address).toString() + ":" + QString::number(port));
            break;
        case Commands::ShortMsg:
            in >> message;
            message |= mClients[ClientAddress(address, port)];
            out << message;
            it = mClients.constBegin();
            while (it != mClients.constEnd()) {
                mReceiver.writeDatagram(datagram, QHostAddress(it.key().address), it.key().port);
                ++it;
            }
            break;
        default:
            ui.teLog->append("Unknown command: " + QString::number(command));
        }
    }
}
