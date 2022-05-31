#include "RoomCreator.h"

#include <QByteArray>
#include <QDataStream>
#include <QNetworkInterface>
#include <QDebug>

QHostAddress RoomCreator::getIpAddress()
{
    const QHostAddress& localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress& address : QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
            return address;
    }
    return QHostAddress::LocalHost;
}

RoomCreator::RoomCreator(quint16 port, QWidget *parent)
    : QMainWindow(parent), mPort(port)
{
    for (int i = MinPort; i < MaxPort; ++i)
        mAvailablePorts.push(i);

    ui.setupUi(this);

    // TODO ������ ���� ��� ��������� ������������� ������
    mReceiver.bind(mHostAddress, mPort);
    connect(&mReceiver, &QUdpSocket::readyRead, this, &RoomCreator::createRoom);

    ui.teLog->append("Server: " + mHostAddress.toString() + ":" + QString::number(mPort));
}

void RoomCreator::createRoom()
{
    QHostAddress clientAddress;
    quint16 clientPort;
    QByteArray bytes(mReceiver.pendingDatagramSize(), '\0');
    mReceiver.readDatagram(bytes.data(), mReceiver.pendingDatagramSize(), &clientAddress, &clientPort);
    
    if (mAvailablePorts.empty()) {
        ui.teLog->append("Cannot create a new room, limit is reached");
        return;
    }
    mRooms[mAvailablePorts.top()] = new Room(mHostAddress, mAvailablePorts.top(), this);
    ui.teLog->append("Created: room: " + mHostAddress.toString() + ":" + QString::number(mAvailablePorts.top()));
    connect(mRooms[mAvailablePorts.top()], &Room::destroyRoom, this, &RoomCreator::destroyRoom);

    QByteArray configuration;
    QDataStream out(&configuration, QIODevice::WriteOnly);
    out << mAvailablePorts.top()
        << MaxRoomDowntime;
    mReceiver.writeDatagram(configuration, clientAddress, clientPort);

    mAvailablePorts.pop();
}

void RoomCreator::destroyRoom(quint16 port)
{
    mRooms.remove(port);
    mAvailablePorts.push(port);
    ui.teLog->append("Destroyed: room " + mHostAddress.toString() + ":" + QString::number(port));
}