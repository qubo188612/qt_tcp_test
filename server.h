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

#define LINK_MODBUS_TCP     0
#define LINK_NORMAL_ASCII   1
#define LINK_NORMAL_RTU     2


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
    QTcpServer * listen;
    QTcpSocket * conn;
    modbus_t * ctx;
    int sock;

    uint16_t mod_registers[MODBUS_MAX_READ_REGISTERS];

    Ui::Server *ui;

    void ReceiveMsg(QByteArray array,QByteArray *sent_array);

private slots:
    void init_show_registers_list();

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
