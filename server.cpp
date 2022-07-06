#include "server.h"
#include "ui_server.h"

#undef HAVE_ACCEPT4

Server::Server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);

    thread1 = new modbustcpThread(this);

    link_mod=LINK_MODBUS_TCP;
    server_state=0;

    ui->radio_1->setChecked(1);

    //设置默认地址和端口
    ui->ip->setText("127.0.0.1");
    ui->port->setText(QString::number(9999)); // QString::number(9999)

    ui->radio_1->setChecked(1);
    connect(ui->radio_1,&QRadioButton::clicked,[=](){
        link_mod=LINK_MODBUS_TCP;
    });
    connect(ui->radio_2,&QRadioButton::clicked,[=](){
        link_mod=LINK_NORMAL_ASCII;
    });
    connect(ui->radio_3,&QRadioButton::clicked,[=](){
        link_mod=LINK_NORMAL_RTU;
    });

    mb_mapping = modbus_mapping_new(
        MODBUS_MAX_READ_BITS, 0,
        MODBUS_MAX_READ_REGISTERS, 0);

    memset(mod_registers,0,sizeof(u_int16_t)*MODBUS_MAX_READ_REGISTERS);

    //创建监听套接字
    connect(ui->startserverBtn,&QPushButton::clicked,[=](){
        if(link_mod==LINK_NORMAL_ASCII||link_mod==LINK_NORMAL_RTU)
        {
            if(server_state==0)
            {
                server_state=1;
                //设置监听
                listen  = new QTcpServer(this);
                listen->listen(QHostAddress(ui->ip->text()), ui->port->text().toInt());

                // 监听新链接
                connect(listen,&QTcpServer::newConnection,[=](){
                    //Returns the next pending connection as a connected QTcpSocket object.
                    conn = listen->nextPendingConnection(); // 沟通的桥梁  通信套接字

                    ui->record->append("有新的连接了");

                    // 保证conn对象有效,所以放到新连接这里
                    // 接收数据
                    connect(conn,&QTcpSocket::readyRead,[=](){
                        if(link_mod==LINK_NORMAL_ASCII)
                        {
                            QByteArray array = conn->readAll();
                            ui->record->append(array);
                            //解码
                        }
                        else if(link_mod==LINK_NORMAL_RTU)
                        {
                            QByteArray array = conn->readAll();
                            QString msg;
                            for(int n=0;n<array.size();n++)
                            {
                                uint8_t data=array[n];
                                msg=msg+"0x"+QString::number(data,16)+" ";
                            }
                            ui->record->append(msg);
                            //解码
                        }
                    });

                });
                ui->startserverBtn->setText("关闭服务器");
                ui->radio_1->setEnabled(false);
                ui->radio_2->setEnabled(false);
                ui->radio_3->setEnabled(false);
            }
            else
            {
                server_state=0;
                listen->close();
                delete listen;
                ui->startserverBtn->setText("启动服务器");
                ui->radio_1->setEnabled(true);
                ui->radio_2->setEnabled(true);
                ui->radio_3->setEnabled(true);
            }
        }
        else if(link_mod==LINK_MODBUS_TCP)
        {
            if(server_state==0)
            {
                server_state=1;
                thread1->start();
                ui->startserverBtn->setText("关闭服务器");
                ui->radio_1->setEnabled(false);
                ui->radio_2->setEnabled(false);
                ui->radio_3->setEnabled(false);
            }
            else
            {
                server_state=0;
                ui->startserverBtn->setText("启动服务器");
                ui->radio_1->setEnabled(true);
                ui->radio_2->setEnabled(true);
                ui->radio_3->setEnabled(true);
            }
        }
    });

    connect(ui->sendBtn,&QPushButton::clicked,[=](){
        if(link_mod==LINK_NORMAL_ASCII)
        {
            QString msg = ui->msgEdit->toPlainText();

            conn->write(msg.toUtf8()); // 发送数据

            ui->record->append("my say:" + msg); // 将数据显示到记录框
        //  ui->msgEdit->clear(); //清空输入框
        }
        else if(link_mod==LINK_NORMAL_RTU)
        {
            QString msg = ui->msgEdit->toPlainText();
            QStringList msgList = msg.split(" ");
            if(msgList.size()<1)
            {
                fprintf(stderr, "msg is too short\n",NULL);
            }
            else
            {
                QByteArray send_group;
                QString showmsg;
                for(int n=0;n<msgList.size();n++)
                {
                    std::string str=msgList[n].toStdString();
                    uint8_t send_data=std::stoi(str,NULL,0);
                    send_group.push_back(send_data);
                    showmsg=showmsg+"0x"+QString::number(send_data,16)+" ";
                }
                conn->write(send_group);
                ui->record->append("my say:" + showmsg); // 将数据显示到记录框
            }
        }
    });
}

Server::~Server()
{
    if(server_state==1)
    {
        server_state=0;
        thread1->quit();
    }
    /*
    modbus_mapping_free(mb_mapping);
    delete thread1;
    delete ui;
    */
}

modbustcpThread::modbustcpThread(Server *statci_p)
{
    _p=statci_p;
}

void modbustcpThread::run()
{
    while(_p->server_state==1)
    {
        QString server_ip=_p->ui->ip->text();
        QString server_port=_p->ui->port->text();
        _p->ctx = modbus_new_tcp(server_ip.toUtf8(), server_port.toInt());
        _p->sock = modbus_tcp_listen(_p->ctx, 1);//最大监听1路
        modbus_tcp_accept(_p->ctx, &_p->sock);
        while(_p->server_state==1)
        {
            int rc;
            uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
            rc = modbus_receive(_p->ctx, query);
            if (rc > 0) {
              modbus_reply(_p->ctx, query, rc, _p->mb_mapping);
              for(int n=0;n<MODBUS_MAX_READ_REGISTERS;n++)
              {
                if(_p->mb_mapping->tab_registers[n]!=_p->mod_registers[n])
                {
                    _p->mod_registers[n]=_p->mb_mapping->tab_registers[n];
                    QString msg=QString::number(n,16);
                    QString data=QString::number(_p->mb_mapping->tab_registers[n],16);
                    _p->ui->record->append("ChangeRegister:[0x"+msg+"]: "+data);
                }
              }
            }
            else if (rc == -1) {
              /* Connection closed by the client or error */
              break;
            }

        }

        close(_p->sock);
        modbus_close(_p->ctx);
        modbus_free(_p->ctx);
    }
}
