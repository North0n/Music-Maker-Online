#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Server.h"

#include <QUdpSocket>
#include <QHostAddress>
#include <QSet>
#include <QPair>
#include <queue>
#include <QTimer>

class Server : public QMainWindow
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
        EstablishConnection,
        ShortMsg,
        Quit,
    };

    Server(quint16 port, QWidget *parent = Q_NULLPTR);

    void receiveData();

    static QHostAddress getIpAddress();

private:
    Ui::ServerClass ui;

    std::priority_queue<quint8, std::vector<quint8>, std::greater<>> mAvailableChannels;
    QHostAddress mHostAddress{ "192.168.0.107" };
    quint16 mPort;
    QUdpSocket mReceiver;
    // Client (address, port) => channel
    QMap<ClientAddress, quint8> mClients;
    // Channel => current instrument
    QMap<quint8, quint32> mChannels;
};
