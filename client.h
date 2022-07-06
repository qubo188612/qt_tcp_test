#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <modbus/modbus.h>

#define LINK_MODBUS_TCP     0
#define LINK_NORMAL_ASCII   1
#define LINK_NORMAL_RTU     2

#define MODBUS_ADD_POS      30
#define MODBUS_ADD_MOVETO   50

namespace Ui {
class Client;
}

class Client : public QWidget
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);
    ~Client();

public:
    QTcpSocket *client;
    modbus_t *ctx;

private:
    Ui::Client *ui;

    uint8_t link_state;
    uint8_t link_mod;

    void ReceiveMsg(QByteArray msg);
};

#endif // CLIENT_H
