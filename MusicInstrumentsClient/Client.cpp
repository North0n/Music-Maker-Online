#include "Client.h"

Client::Client(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    fillInstruments();
}

void Client::fillInstruments()
{
    for (int i = 0; i < Instruments.count(); ++i) {
        ui.cbInstrument->addItem(Instruments[i], i);
    }
}

void Client::on_actConnect_triggered()
{
    mFormConnection = std::make_unique<FormConnection>(this);
    connect(mFormConnection.get(), &FormConnection::onConnectPressed, ui.wPiano, &Piano::connectToServer);
    mFormConnection->show();
}

void Client::on_actCreate_triggered()
{
    mFormCreation = std::make_unique<FormCreation>(this);
    connect(mFormCreation.get(), &FormCreation::onCreatePressed, ui.wPiano, &Piano::createRoom);
    mFormCreation->show();
}

void Client::on_actDisconnect_triggered()
{
    ui.wPiano->disconnectFormServer();
}
