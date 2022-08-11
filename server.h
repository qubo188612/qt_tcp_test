#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <modbus/modbus.h>
#include <QThread>
#include <unistd.h>
#include <global.h>
#include <QObject>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <math.h>
#include <stdio.h>


QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class modbustcpThread;

class Server : public QWidget
{
    Q_OBJECT

public:
    Server(QWidget *parent = nullptr);
    ~Server();

    uint8_t server_state;
    uint8_t link_mod;
    modbus_mapping_t * mb_mapping;
    modbustcpThread *thread1;

public:
    u_int16_t robot_mod;

    QTcpServer * listen;
    QTcpSocket * conn;
    modbus_t * ctx;
    int sock;

    uint16_t mod_registers[MODBUS_REGISTERS_MAXNUM];

    Ui::Server *ui;

    void ReceiveMsg(QByteArray array,QByteArray *sent_array);

    QString JsonToQstring(QJsonObject jsonObject);

    QJsonObject QstringToJson(QString jsonString);

    void Round(float f_in,float *f_out,int decimalplace);//保留小数位数

private slots:
    void init_show_registers_list();

    void on_comboBox_currentIndexChanged(int index);
};

class modbustcpThread : public QThread
{
    Q_OBJECT

public:
    modbustcpThread(Server *statci_p);
protected:
    void run();
private:
    Server *_p;

signals:
    // 自定义信号
    void Send_show_registers_list();

};
#endif // SERVER_H
