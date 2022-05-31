#include "RoomCreator.h"
#include <QtWidgets/QApplication>
#include <QHostAddress>
#include <QString>

quint16 port = 20000;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        qDebug() << "You should specify ip address for the server as first command line argument";
        return -1;
    }
    if (argc == 3) {
        port = QString(argv[2]).toInt();
    }


    QApplication a(argc, argv);
    RoomCreator w(QHostAddress(argv[1]), port);
    w.show();
    return a.exec();
}
