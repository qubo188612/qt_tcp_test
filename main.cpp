#include <QApplication>
#include "server.h"
#include "client.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Server w;
    w.setWindowTitle("服务端");
    w.show();

    Client c;
    c.setWindowTitle("客户端");
    c.show();

    return a.exec();
}
