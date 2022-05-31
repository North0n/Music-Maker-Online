#pragma once
#include <QUdpSocket>
#include <QHostAddress>
#include <QSet>
#include <QPair>
#include <queue>
#include <QTimer>

class Room : public QObject
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
    
    Room(const QHostAddress &address, quint16 port, QObject* parent = Q_NULLPTR);

    void receiveData();

    // TODO add disconnect on timer

signals:
    void destroyRoom(quint16 port);

private:
    QHostAddress mMyAddress;
    quint16 mMyPort;

    QUdpSocket mReceiver;
    std::priority_queue<quint8, std::vector<quint8>, std::greater<>> mAvailableChannels;
    // Client (address, port) => channel
    QMap<ClientAddress, quint8> mClients;
    // Channel => current instrument
    QMap<quint8, quint32> mChannels;
};

