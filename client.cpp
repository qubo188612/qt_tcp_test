#include "client.h"
#include "ui_client.h"

Client::Client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);

    //设置默认地址和端口
    ui->ip->setText("127.0.0.1");
    ui->port->setText(QString::number(9999)); // QString::number(9999)

    client = new QTcpSocket(this);

    link_state=0;
    link_mod=LINK_MODBUS_TCP;

    ui->radio_1->setChecked(1);
    connect(ui->radio_1,&QRadioButton::clicked,[=](){
        link_mod=LINK_MODBUS_TCP;
        ui->sendBtn_2->show();
    });
    connect(ui->radio_2,&QRadioButton::clicked,[=](){
        link_mod=LINK_NORMAL_ASCII;
        ui->sendBtn_2->hide();
    });
    connect(ui->radio_3,&QRadioButton::clicked,[=](){
        link_mod=LINK_NORMAL_RTU;
        ui->sendBtn_2->hide();
    });

    ui->edit_pos_X->setText("61.606");
    ui->edit_pos_Y->setText("51.505");
    ui->edit_pos_Z->setText("41.404");
    ui->edit_pos_RX->setText("31.303");
    ui->edit_pos_RY->setText("21.202");
    ui->edit_pos_RZ->setText("11.101");
    ui->edit_pos_SP->setText("9.999");

    ui->sendBtn->setEnabled(false);
    ui->sendBtn_2->setEnabled(false);
    ui->getposBtn->setEnabled(false);
    ui->moveBtn->setEnabled(false);

    //连接服务器
    connect(ui->linkBtn,&QPushButton::clicked,[=](){
        if(link_mod==LINK_NORMAL_ASCII||link_mod==LINK_NORMAL_RTU)
        {
            if(link_state==0)
            {
                link_state=1;
                client->connectToHost(ui->ip->text(),ui->port->text().toInt());
                ui->linkBtn->setText("断开");
                ui->radio_1->setEnabled(false);
                ui->radio_2->setEnabled(false);
                ui->radio_3->setEnabled(false);
                ui->sendBtn->setEnabled(true);
                ui->sendBtn_2->setEnabled(true);
                ui->getposBtn->setEnabled(true);
                ui->moveBtn->setEnabled(true);
            }
            else
            {
                link_state=0;
                client->disconnectFromHost();
                ui->linkBtn->setText("连接");
                ui->radio_1->setEnabled(true);
                ui->radio_2->setEnabled(true);
                ui->radio_3->setEnabled(true);
                ui->sendBtn->setEnabled(false);
                ui->sendBtn_2->setEnabled(false);
                ui->getposBtn->setEnabled(false);
                ui->moveBtn->setEnabled(false);
            }
        }
        else if(link_mod==LINK_MODBUS_TCP)
        {
            if(link_state==0)
            {
                QString server_ip=ui->ip->text();
                QString server_port=ui->port->text();
                ctx = modbus_new_tcp(server_ip.toUtf8(), server_port.toInt());
                if (modbus_connect(ctx) == -1)
                {
                    fprintf(stderr, "Connection failed: %s\n",modbus_strerror(errno));
                    modbus_free(ctx);
                }
                else
                {
                    link_state=1;
                    ui->linkBtn->setText("断开");
                    ui->radio_1->setEnabled(false);
                    ui->radio_2->setEnabled(false);
                    ui->radio_3->setEnabled(false);
                    ui->sendBtn->setEnabled(true);
                    ui->sendBtn_2->setEnabled(true);
                    ui->getposBtn->setEnabled(true);
                    ui->moveBtn->setEnabled(true);
                }
            }
            else
            {
                link_state=0;
                modbus_close(ctx);
                modbus_free(ctx);
                ui->linkBtn->setText("连接");
                ui->radio_1->setEnabled(true);
                ui->radio_2->setEnabled(true);
                ui->radio_3->setEnabled(true);
                ui->sendBtn->setEnabled(false);
                ui->sendBtn_2->setEnabled(false);
                ui->getposBtn->setEnabled(false);
                ui->moveBtn->setEnabled(false);
            }
        }
    });

    //发送编辑框数据
    connect(ui->sendBtn,&QPushButton::clicked,[=](){
        if(link_mod==LINK_NORMAL_ASCII)
        {
            QString msg = ui->msgEdit->toPlainText();
            client->write(msg.toUtf8());
            ui->record->append("my say:"+ msg);
        }
        else if(link_mod==LINK_MODBUS_TCP)
        {
            int rc;
            QString msg = ui->msgEdit->toPlainText();
            QStringList msgList = msg.split(" ");
            if(msgList.size()<=1)
            {
                fprintf(stderr, "msg is too short\n",NULL);
            }
            else
            {
                u_int16_t *send_num_buffer=new u_int16_t[msgList.size()-1];
                int send_addr;
                for(int n=0;n<msgList.size();n++)
                {
                    std::string str=msgList[n].toStdString();
                    if(n==0)
                    {
                        send_addr=std::stoi(str,NULL,0);
                    }
                    else
                    {
                        send_num_buffer[n-1]=std::stoi(str,NULL,0);
                    }
                }
                int write_num=msgList.size()-1;
                rc=modbus_write_registers(ctx,send_addr,write_num,send_num_buffer);
                if(rc!=1)
                {
                    fprintf(stderr, "write registers failed\n",NULL);
                }
                delete [] send_num_buffer;
            }
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
                client->write(send_group);
                ui->record->append("my say:" + showmsg); // 将数据显示到记录框
            }
        }
     // ui->msgEdit->clear();
    });

    //发送编辑框数据
    connect(ui->sendBtn_2,&QPushButton::clicked,[=](){
        if(link_mod==LINK_MODBUS_TCP)
        {
            QString msg = ui->msgEdit->toPlainText();
            QStringList msgList = msg.split(" ");
            if(msgList.size()!=2)
            {
                fprintf(stderr, "msg is too short\n",NULL);
            }
            else
            {
                u_int16_t *red_registers=new u_int16_t[MODBUS_MAX_READ_REGISTERS];
                int send_addr;
                int readnum;
                int real_readnum;
                for(int n=0;n<msgList.size();n++)
                {
                    std::string str=msgList[n].toStdString();
                    if(n==0)
                    {
                        send_addr=std::stoi(str,NULL,0);
                    }
                    else
                    {
                        readnum=std::stoi(str,NULL,0);
                    }
                }
                real_readnum=modbus_read_registers(ctx,send_addr,readnum,red_registers);
                if(real_readnum<0)
                {
                    fprintf(stderr, "write registers failed\n",NULL);
                }
                else
                {
                    for(int m=0;m<real_readnum;m++)
                    {
                        int n=send_addr+m;
                        QString msg=QString::number(n,16);
                        QString data=QString::number(red_registers[m],16);
                        ui->record->append("Register:[0x"+msg+"]: "+data);
                    }
                }
                delete [] red_registers;
            }
        }
    });

    //获取坐标
    connect(ui->getposBtn,&QPushButton::clicked,[=](){
        if(link_mod==LINK_NORMAL_ASCII)
        {
            QString msg=ASK_GETPOS_ASCII;
            //这里添加获取坐标信息
            client->write(msg.toUtf8());
        }
        else if(link_mod==LINK_MODBUS_TCP)
        {
            int nb_points=12;
            uint16_t *tab_reg=new uint16_t[nb_points];
            int rc = modbus_read_registers(ctx, MODBUS_ADD_POS, nb_points, tab_reg);
            if(rc!=nb_points)
            {
                fprintf(stderr, "read pose failed\n",NULL);
            }
            else
            {
                float *f_tab_reg=(float*)tab_reg;
                float posX=f_tab_reg[0];
                float posY=f_tab_reg[1];
                float posZ=f_tab_reg[2];
                float posRX=f_tab_reg[3];
                float posRY=f_tab_reg[4];
                float posRZ=f_tab_reg[5];
                QString s_pos;
                s_pos="("+QString::number(posX)+","+QString::number(posY)+","+QString::number(posZ)+","
                        +QString::number(posRX)+","+QString::number(posRY)+","+QString::number(posRZ)+")";
                ui->strpos->setText(s_pos);
            }
            delete[] tab_reg;
        }
        else if(link_mod==LINK_NORMAL_RTU)
        {
            const char cart[] = ASK_GETPOS_RTU;
            QByteArray send_group(QByteArray::fromRawData(cart, sizeof(cart)));
            client->write(send_group);
        }
    });

    //发送移动指令
    connect(ui->moveBtn,&QPushButton::clicked,[=](){
        if(link_mod==LINK_NORMAL_ASCII)
        {
            QString msg_Posx=ui->edit_pos_X->text();
            QString msg_Posy=ui->edit_pos_Y->text();
            QString msg_Posz=ui->edit_pos_Z->text();
            QString msg_Posrx=ui->edit_pos_RX->text();
            QString msg_Posry=ui->edit_pos_RY->text();
            QString msg_Posrz=ui->edit_pos_RZ->text();
            QString msg_Possp=ui->edit_pos_SP->text();

            QString msgtemp1=ASK_MOVETO_ASCII;
            QString msgtemp2=ASK_MOVETOSP_ASCII;
            QString msg=msgtemp1+" ("+msg_Posx+","+msg_Posy+","+msg_Posz+","+msg_Posrx+","+msg_Posry+","+msg_Posrz+") "+msgtemp2+msg_Possp;
            //这里添加获取坐标信息
            client->write(msg.toUtf8());
        }
        else if(link_mod==LINK_MODBUS_TCP)
        {
            float PosX=ui->edit_pos_X->text().toFloat();
            float PosY=ui->edit_pos_Y->text().toFloat();
            float PosZ=ui->edit_pos_Z->text().toFloat();
            float PosRX=ui->edit_pos_RX->text().toFloat();
            float PosRY=ui->edit_pos_RY->text().toFloat();
            float PosRZ=ui->edit_pos_RZ->text().toFloat();
            float Speed=ui->edit_pos_SP->text().toFloat();
            int nb_points=7;
            float *f_tab_reg=new float[nb_points];
            uint16_t *tab_reg=(uint16_t*)f_tab_reg;
            f_tab_reg[0]=PosX;
            f_tab_reg[1]=PosY;
            f_tab_reg[2]=PosZ;
            f_tab_reg[3]=PosRX;
            f_tab_reg[4]=PosRY;
            f_tab_reg[5]=PosRZ;
            f_tab_reg[6]=Speed;
            int rc=modbus_write_registers(ctx,MODBUS_ADD_MOVETO,nb_points*2,tab_reg);
            if(rc!=1)
            {
                fprintf(stderr, "write registers failed\n",NULL);
            }
            delete []f_tab_reg;
        }
        else if(link_mod==LINK_NORMAL_RTU)
        {
            float PosX=ui->edit_pos_X->text().toFloat();
            float PosY=ui->edit_pos_Y->text().toFloat();
            float PosZ=ui->edit_pos_Z->text().toFloat();
            float PosRX=ui->edit_pos_RX->text().toFloat();
            float PosRY=ui->edit_pos_RY->text().toFloat();
            float PosRZ=ui->edit_pos_RZ->text().toFloat();
            float Speed=ui->edit_pos_SP->text().toFloat();
            int nb_points=7;
            float *f_tab_reg=new float[nb_points];
            uint8_t *tab_reg=(uint8_t*)f_tab_reg;
            f_tab_reg[0]=PosX;
            f_tab_reg[1]=PosY;
            f_tab_reg[2]=PosZ;
            f_tab_reg[3]=PosRX;
            f_tab_reg[4]=PosRY;
            f_tab_reg[5]=PosRZ;
            f_tab_reg[6]=Speed;
            QByteArray send_group;
            send_group.resize(nb_points*4);
            for(int n=0;n<send_group.size();n++)
            {
                send_group[n]=tab_reg[n];
            }
            char getmovetoId[]=ASK_MOVETO_RTU;
            QByteArray QgetmovetoId(QByteArray::fromRawData(getmovetoId, sizeof(getmovetoId)));
            send_group=QgetmovetoId+send_group;
            client->write(send_group);
        }
    });

    //客户端接收数据
    connect(client,&QTcpSocket::readyRead,[=](){
        if(link_mod==LINK_NORMAL_ASCII)
        {
            QByteArray array = client->readAll();
            ui->record->append(array);
            ReceiveMsg(array);
        }
        else if(link_mod==LINK_NORMAL_RTU)
        {
            QByteArray array = client->readAll();
            QString msg;
            for(int n=0;n<array.size();n++)
            {
                uint8_t data=array[n];
                msg=msg+"0x"+QString::number(data,16)+" ";
            }
            ui->record->append(msg);
            ReceiveMsg(array);
        }
    });

}


Client::~Client()
{
    delete ui;
}

void Client::ReceiveMsg(QByteArray msg)
{
    if(msg.size()!=0)
    {
        if(link_mod==LINK_NORMAL_ASCII)
        {
            QString recv_msg = msg;
            QStringList msgList = recv_msg.split(" ");
            if(msgList.size()<1)
            {
                fprintf(stderr, "msg is too short\n",NULL);
            }
            else
            {
                if(msgList[0]==ASE_GETPOS_ASCII)//坐标回复
                {
                    if(msgList.size()<7)
                    {
                        fprintf(stderr, "msg is too short\n",NULL);
                    }
                    else
                    {
                        float posX=msgList[1].toFloat();
                        float posY=msgList[2].toFloat();
                        float posZ=msgList[3].toFloat();
                        float posRX=msgList[4].toFloat();
                        float posRY=msgList[5].toFloat();
                        float posRZ=msgList[6].toFloat();
                        QString s_pos;
                        s_pos="("+QString::number(posX)+","+QString::number(posY)+","+QString::number(posZ)+","
                                +QString::number(posRX)+","+QString::number(posRY)+","+QString::number(posRZ)+")";
                        ui->strpos->setText(s_pos);

                    }
                }
           //   else if(msgList[0]==)   //其他回复
           //   {
           //
           //   }
            }
        }
        else if(link_mod==LINK_NORMAL_RTU)
        {
            //解码
            char rcvId[RTU_MARK_ID_NUM];
            strncpy(rcvId, msg.data(),RTU_MARK_ID_NUM);
            QByteArray recv_Id(QByteArray::fromRawData(rcvId, sizeof(rcvId)));

            char getposId[]=ASE_GETPOS_RTU;
            QByteArray QgetposId(QByteArray::fromRawData(getposId, sizeof(getposId)));
            if(recv_Id==QgetposId)//坐标回复
            {
                if(msg.size()<6*sizeof(float)+RTU_MARK_ID_NUM)
                {
                    fprintf(stderr, "msg is too short\n",NULL);
                }
                else
                {
                    uint8_t posdata[6*sizeof(float)];
                    float *f_pos=(float*)posdata;
                    memcpy(posdata,msg.data()+RTU_MARK_ID_NUM,6*sizeof(float));
                    float posX=f_pos[0];
                    float posY=f_pos[1];
                    float posZ=f_pos[2];
                    float posRX=f_pos[3];
                    float posRY=f_pos[4];
                    float posRZ=f_pos[5];
                    QString s_pos;
                    s_pos="("+QString::number(posX)+","+QString::number(posY)+","+QString::number(posZ)+","
                            +QString::number(posRX)+","+QString::number(posRY)+","+QString::number(posRZ)+")";
                    ui->strpos->setText(s_pos);
                }
            }

         // char get？[]=;
         // QByteArray Qget?(QByteArray::fromRawData(get？, sizeof(get？)));
         // if(recv_Id==Qget?)//其他回复
         // {

         // }
        }
    }
}
