#include "Client.h"
#include <QtWidgets/QApplication>

#include "Windows.h"
#include "mmeapi.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client w;
    w.show();
    return a.exec();
}
