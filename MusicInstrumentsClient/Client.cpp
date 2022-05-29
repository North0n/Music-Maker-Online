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
