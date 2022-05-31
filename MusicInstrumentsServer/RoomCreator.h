#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Server.h"

#include "Room.h"
#include <QUdpSocket>
#include <QHostAddress>
#include <QSet>
#include <QPair>
#include <queue>
#include <QTimer>

class RoomCreator : public QMainWindow
{
    Q_OBJECT

public:

    struct ClientAddress
    {
        ClientAddress(const QHostAddress& Address, quint16 Port)
            : address(Address.toIPv4Address()), port(Port)
        {}

        ClientAddress() = default;

        quint32 address;
        quint16 port;

        auto operator<(const ClientAddress& client) const
        {
            return std::tie(this->address, this->port) < std::tie(client.address, client.port);
        }

        auto operator>(const ClientAddress& client) const
        {
            return std::tie(this->address, this->port) > std::tie(client.address, client.port);
        }

        auto operator==(const ClientAddress& client) const
        {
            return std::tie(this->address, this->port) == std::tie(client.address, client.port);
        }
    };

    enum Commands
    {
        CreateRoom,
        EstablishConnection,
        ShortMsg,
        Quit,
    };

    RoomCreator(quint16 port, QWidget *parent = Q_NULLPTR);

    static QHostAddress getIpAddress();

private slots:

    void createRoom();

    void destroyRoom(quint16 port);

private:

    const quint16 MinPort = 30000;
    const quint16 MaxPort = 40000;
    const quint16 MaxRoomDowntime = 60 * 5;

    Ui::ServerClass ui;

    std::priority_queue<quint16, std::vector<quint16>, std::greater<>> mAvailablePorts;
    QMap<quint16, Room*> mRooms;

    QHostAddress mHostAddress{ "192.168.0.107" };
    quint16 mPort;
    QUdpSocket mReceiver;
};
