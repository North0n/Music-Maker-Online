#include "Room.h"

#include <QStack>

Room::Room(const QHostAddress& address, quint16 port, quint16 maxDowntime, QObject* parent)
    : mMyAddress(address), mMyPort(port), mMaxDowntime(maxDowntime), mTimer(this)
{
    for (int i = 0; i < 10; ++i)
        mAvailableChannels.push(i);

    mReceiver.bind(mMyAddress, mMyPort);
    connect(&mReceiver, &QUdpSocket::readyRead, this, &Room::receiveData);
    connect(&mTimer, &QTimer::timeout, this, &Room::checkConnection);
    mTimer.start(mMaxDowntime);

    emit logMessage("Waiting for connections on IP: " + mMyAddress.toString() + ":" + QString::number(mMyPort));
}

void Room::receiveData()
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
    while (!in.atEnd()) {
        in >> command;
        switch (command)
        {
        case Commands::EstablishConnection:
            if (mAvailableChannels.empty())
                return;
            mClients[ClientAddress(address, port)] = ClientData(mAvailableChannels.top());
            mAvailableChannels.pop();
            in >> instrument;
            mChannels[mClients[ClientAddress(address, port)].channel] = instrument;
            emit logMessage("Connected: " + QHostAddress(address).toString() + ":" + QString::number(port));

            // Tell client that connection was succesfull
            out << static_cast<quint8>(Commands::EstablishConnection);
            // Send info about current instruments of already connected cliens
            for (auto it = mChannels.constBegin(); it != mChannels.constEnd(); ++it) {
                out << static_cast<quint8>(Commands::ShortMsg) << static_cast<quint32>(0x0000C0 | (it.value() << 8) | it.key());
            }
            mReceiver.writeDatagram(datagram, address, port);
            break;
        case Commands::Quit:
            emit logMessage("Disconnected: " + QHostAddress(address).toString() + ":" + QString::number(port));
            disconnectClient(ClientAddress(address, port));
            break;
        case Commands::ShortMsg:
            mClients[ClientAddress(address, port)].sentCommand = true;

            in >> message;
            // Change the instrument for the channel
            if ((message & 0xFF) == 0xC0) {
                mChannels[mClients[ClientAddress(address, port)].channel] = (message & 0xFF00) >> 8;
            }
            message |= mClients[ClientAddress(address, port)].channel;

            // Send message to each connected client
            out << static_cast<quint8>(Commands::ShortMsg) << message;
            for (auto it = mClients.begin(); it != mClients.end(); ++it) {
                mReceiver.writeDatagram(datagram, QHostAddress(it.key().address), it.key().port);
            }
            break;
        default:
            emit logMessage("Unknown command: " + QString::number(command));
            //ui.teLog->append("Unknown command: " + QString::number(command));
        }
    }
}

void Room::disconnectClient(const ClientAddress& client)
{
    mAvailableChannels.push(mClients[client].channel);
    mClients.remove(client);

    // If last client disconnected then destroy the room
    if (mAvailableChannels.size() == 10) {
        emit logMessage("Last client left. Room " + mMyAddress.toString() + ":" + QString::number(mMyPort) + " is destroyed.");
        emit destroyRoom(mMyPort);
    }
}

void Room::checkConnection()
{
    QStack<ClientAddress> clientsForDeletion;

    for (auto it = mClients.begin(); it != mClients.end(); ++it) {
        if (!it.value().sentCommand) {
            clientsForDeletion.push(it.key());
        }
        it.value().sentCommand = false;
    }

    while (!clientsForDeletion.isEmpty()) {
        auto client = clientsForDeletion.pop();
        // Send message to client about his disconnection
        QByteArray datagram;
        QDataStream out(&datagram, QIODevice::WriteOnly);
        out << static_cast<quint8>(Commands::Quit);
        mReceiver.writeDatagram(datagram, QHostAddress(client.address), client.port);

        emit logMessage("Disconnected: " + QHostAddress(client.address).toString() + ":" + QString::number(client.port) + " due to long timeout");
        disconnectClient(client);
    }
}