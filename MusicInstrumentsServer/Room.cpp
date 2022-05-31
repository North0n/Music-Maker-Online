#include "Room.h"

Room::Room(const QHostAddress& address, quint16 port, QObject* parent)
    : mMyAddress(address), mMyPort(port)
{
    for (int i = 0; i < 10; ++i)
        mAvailableChannels.push(i);

    // TODO менять бинд при изменении пользователем адреса
    mReceiver.bind(mMyAddress, mMyPort);
    connect(&mReceiver, &QUdpSocket::readyRead, this, &Room::receiveData);

    // TODO возможно добавить сигнал который будет говорить родителю вот типа жду, а потом вот типа чел подключился и т.д.
    qInfo() << "Waiting for connections on IP: " + mMyAddress.toString() + ":" + QString::number(mMyPort);
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
    ClientAddress client;
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
            qInfo() << "Connected: " + QHostAddress(address).toString() + ":" + QString::number(port);
            //ui.teLog->append("Connected: " + QHostAddress(address).toString() + ":" + QString::number(port));

            // Send info about current instruments of already connected cliens
            for (auto it = mChannels.constBegin(); it != mChannels.constEnd(); ++it) {
                out << static_cast<quint32>(0x0000C0 | (it.value() << 8) | it.key());
            }
            mReceiver.writeDatagram(datagram, address, port);
            break;
        case Commands::Quit:
            client = ClientAddress(address, port);
            mAvailableChannels.push(mClients[client]);
            mClients.remove(client);
            qInfo() << "Disconnected: " + QHostAddress(address).toString() + ":" + QString::number(port);

            // If last client disconnected the destroy the room
            if (mAvailableChannels.size() == 10) {
                emit destroyRoom(mMyPort);
                qInfo() << "Last client left. Room " + QHostAddress(address).toString() + ":" + QString::number(port) + " is destroyed.";
            }
            //ui.teLog->append("Disconnected: " + QHostAddress(address).toString() + ":" + QString::number(port));
            break;
        case Commands::ShortMsg:
            in >> message;
            if ((message & 0xFF) == 0xC0) {
                mChannels[mClients[ClientAddress(address, port)]] = (message & 0xFF00) >> 8;
            }
            message |= mClients[ClientAddress(address, port)];
            out << message;
            for (auto it = mClients.constBegin(); it != mClients.constEnd(); ++it) {
                qInfo() << QHostAddress(it.key().address).toString() + ":" + QString::number(it.key().port) << (void*)message;
                mReceiver.writeDatagram(datagram, QHostAddress(it.key().address), it.key().port);
            }
            break;
        default:
            qInfo() << "Unknown command: " + QString::number(command);
            //ui.teLog->append("Unknown command: " + QString::number(command));
        }
    }
}
