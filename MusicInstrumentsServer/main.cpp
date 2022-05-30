#include "Server.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server w(20000);
    w.show();
    return a.exec();
}
