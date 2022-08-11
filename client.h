#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <modbus/modbus.h>
#include <global.h>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


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

    u_int16_t robot_mod;

private slots:
    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::Client *ui;

    uint8_t link_state;
    uint8_t link_mod;

    void ReceiveMsg(QByteArray msg);


    QString JsonToQstring(QJsonObject jsonObject);

    QJsonObject QstringToJson(QString jsonString);
};

#endif // CLIENT_H
