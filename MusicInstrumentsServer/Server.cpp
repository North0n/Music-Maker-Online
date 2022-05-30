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
    QDataStream in(&data, QIODevice::ReadOnly), 
                out(&datagram, QIODevice::WriteOnly);
    quint8 command, instrument;
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
            in >> instrument;
            qDebug() << (void*)instrument;
            mChannels[mClients[ClientAddress(address, port)]] = instrument;
            ui.teLog->append("Connected: " + QHostAddress(address).toString() + ":" + QString::number(port));

            // Send info about current instruments of already connected cliens
            out << static_cast<quint8>(Commands::EstablishConnection);
            for (auto it = mChannels.constBegin(); it != mChannels.constEnd(); ++it) {
                out << static_cast<quint32>(0x0000C0 | (it.value() << 8) | it.key());
            }
            mReceiver.writeDatagram(datagram, address, port);
            break;
        case Commands::Quit:
            client = ClientAddress(address, port);
            mAvailableChannels.push(mClients[client]);
            mClients.remove(client);
            ui.teLog->append("Disconnected: " + QHostAddress(address).toString() + ":" + QString::number(port));
            break;
        case Commands::ShortMsg:
            in >> message;
            if ((message & 0xFF) == 0xC0) {
                mChannels[mClients[ClientAddress(address, port)]] = (message & 0xFF00) >> 8;
            }
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
