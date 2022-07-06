#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <modbus/modbus.h>
#include <QThread>
#include <unistd.h>


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
};

class modbustcpThread : public QThread
{
public:
    modbustcpThread(Server *statci_p);
protected:
    void run();
private:
    Server *_p;
};
#endif // SERVER_H
