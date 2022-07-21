#include "client.h"
#include "ui_client.h"

Client::Client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);

    //设置默认地址和端口
    ui->ip->setText("127.0.0.1");
    ui->port->setText(QString::number(1502)); // QString::number(9999)

    client = new QTcpSocket(this);

    link_state=0;
    link_mod=LINK_MODBUS_TCP;

    ui->radio_1->setChecked(1);

    ui->laseropenBtn->hide();
    ui->lasercloseBtn->hide();
    ui->cameraopenBtn->hide();
    ui->cameracloseBtn->hide();
    ui->label_6->hide();
    ui->edit_tasknum->hide();
    ui->tasknumwriteBtn->hide();
    ui->gitallinfoBtn->hide();

    connect(ui->radio_1,&QRadioButton::clicked,[=](){
        link_mod=LINK_MODBUS_TCP;
        ui->sendBtn_2->show();
        ui->laseropenBtn->hide();
        ui->lasercloseBtn->hide();
        ui->cameraopenBtn->hide();
        ui->cameracloseBtn->hide();
        ui->label_6->hide();
        ui->edit_tasknum->hide();
        ui->tasknumwriteBtn->hide();
        ui->gitallinfoBtn->hide();
    });
    connect(ui->radio_2,&QRadioButton::clicked,[=](){
        link_mod=LINK_NORMAL_ASCII;
        ui->sendBtn_2->hide();
        ui->laseropenBtn->hide();
        ui->lasercloseBtn->hide();
        ui->cameraopenBtn->hide();
        ui->cameracloseBtn->hide();
        ui->label_6->hide();
        ui->edit_tasknum->hide();
        ui->tasknumwriteBtn->hide();
        ui->gitallinfoBtn->hide();
    });
    connect(ui->radio_3,&QRadioButton::clicked,[=](){
        link_mod=LINK_NORMAL_RTU;
        ui->sendBtn_2->hide();
        ui->laseropenBtn->hide();
        ui->lasercloseBtn->hide();
        ui->cameraopenBtn->hide();
        ui->cameracloseBtn->hide();
        ui->label_6->hide();
        ui->edit_tasknum->hide();
        ui->tasknumwriteBtn->hide();
        ui->gitallinfoBtn->hide();
    });
    connect(ui->radio_4,&QRadioButton::clicked,[=](){
        link_mod=LINK_KAWASAKI;
        ui->sendBtn_2->hide();
        ui->laseropenBtn->show();
        ui->lasercloseBtn->show();
        ui->cameraopenBtn->show();
        ui->cameracloseBtn->show();
        ui->label_6->show();
        ui->edit_tasknum->show();
        ui->tasknumwriteBtn->show();
        ui->gitallinfoBtn->show();
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
        if(link_mod==LINK_NORMAL_ASCII||link_mod==LINK_NORMAL_RTU||link_mod==LINK_KAWASAKI)
        {
            if(link_state==0)
            {
                link_state=1;
                client->connectToHost(ui->ip->text(),ui->port->text().toInt());
                ui->linkBtn->setText("断开");
                ui->radio_1->setEnabled(false);
                ui->radio_2->setEnabled(false);
                ui->radio_3->setEnabled(false);
                ui->radio_4->setEnabled(false);
                ui->sendBtn->setEnabled(true);
                ui->sendBtn_2->setEnabled(true);
                ui->getposBtn->setEnabled(true);
                ui->moveBtn->setEnabled(true);                
                ui->laseropenBtn->setEnabled(true);
                ui->lasercloseBtn->setEnabled(true);
                ui->cameraopenBtn->setEnabled(true);
                ui->cameracloseBtn->setEnabled(true);
                ui->tasknumwriteBtn->setEnabled(true);
                ui->gitallinfoBtn->setEnabled(true);
            }
            else
            {
                link_state=0;
                client->disconnectFromHost();
                ui->linkBtn->setText("连接");
                ui->radio_1->setEnabled(true);
                ui->radio_2->setEnabled(true);
                ui->radio_3->setEnabled(true);
                ui->radio_4->setEnabled(true);
                ui->sendBtn->setEnabled(false);
                ui->sendBtn_2->setEnabled(false);
                ui->getposBtn->setEnabled(false);
                ui->moveBtn->setEnabled(false);
                ui->laseropenBtn->setEnabled(false);
                ui->lasercloseBtn->setEnabled(false);
                ui->cameraopenBtn->setEnabled(false);
                ui->cameracloseBtn->setEnabled(false);
                ui->tasknumwriteBtn->setEnabled(false);
                ui->gitallinfoBtn->setEnabled(false);
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
                    ui->radio_4->setEnabled(false);
                    ui->sendBtn->setEnabled(true);
                    ui->sendBtn_2->setEnabled(true);
                    ui->getposBtn->setEnabled(true);
                    ui->moveBtn->setEnabled(true);
                    ui->laseropenBtn->setEnabled(true);
                    ui->lasercloseBtn->setEnabled(true);
                    ui->cameraopenBtn->setEnabled(true);
                    ui->cameracloseBtn->setEnabled(true);
                    ui->tasknumwriteBtn->setEnabled(true);
                    ui->gitallinfoBtn->setEnabled(true);
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
                ui->radio_4->setEnabled(true);
                ui->sendBtn->setEnabled(false);
                ui->sendBtn_2->setEnabled(false);
                ui->getposBtn->setEnabled(false);
                ui->moveBtn->setEnabled(false);
                ui->laseropenBtn->setEnabled(false);
                ui->lasercloseBtn->setEnabled(false);
                ui->cameraopenBtn->setEnabled(false);
                ui->cameracloseBtn->setEnabled(false);
                ui->tasknumwriteBtn->setEnabled(false);
                ui->gitallinfoBtn->setEnabled(false);
            }
        }
    });

    //发送编辑框数据
    connect(ui->sendBtn,&QPushButton::clicked,[=](){
        if(link_mod==LINK_NORMAL_ASCII||link_mod==LINK_KAWASAKI)
        {
            QString msg = ui->msgEdit->toPlainText();
            client->write(msg.toUtf8());
            ui->record->append("发送:"+ msg);
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
                ui->record->append("发送:" + showmsg); // 将数据显示到记录框
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
        else if(link_mod==LINK_KAWASAKI)
        {
            QJsonObject json;
         // json.insert(ASK_POS2_KEY_KAWASAKI, ASK_POS2_ONCE_KAWASAKI);
            json.insert(ASK_POS6_KEY_KAWASAKI, ASK_POS6_ONCE_KAWASAKI);
            QString msg=JsonToQstring(json);
            client->write(msg.toUtf8());
            ui->record->append("发送:" + msg); // 将数据显示到记录框
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
        else if(link_mod==LINK_KAWASAKI)
        {

        }
    });

    //开激光指令
    connect(ui->laseropenBtn,&QPushButton::clicked,[=](){
        if(link_mod==LINK_KAWASAKI)
        {
            QJsonObject json;
            json.insert(ASK_LASER_KEY_KAWASAKI, ASK_LASER_OPEN_KAWASAKI);
            QString msg=JsonToQstring(json);
            client->write(msg.toUtf8());
            ui->record->append("发送:" + msg); // 将数据显示到记录框
        }
    });

    //关激光指令
    connect(ui->lasercloseBtn,&QPushButton::clicked,[=](){
        if(link_mod==LINK_KAWASAKI)
        {
            QJsonObject json;
            json.insert(ASK_LASER_KEY_KAWASAKI, ASK_LASER_CLOSE_KAWASAKI);
            QString msg=JsonToQstring(json);
            client->write(msg.toUtf8());
            ui->record->append("发送:" + msg); // 将数据显示到记录框
        }
    });

    //开相机指令
    connect(ui->cameraopenBtn,&QPushButton::clicked,[=](){
        if(link_mod==LINK_KAWASAKI)
        {
            QJsonObject json;
            json.insert(ASK_CAMERA_KEY_KAWASAKI, ASK_CAMERA_OPEN_KAWASAKI);
            QString msg=JsonToQstring(json);
            client->write(msg.toUtf8());
            ui->record->append("发送:" + msg); // 将数据显示到记录框
        }
    });

    //关相机指令
    connect(ui->cameracloseBtn,&QPushButton::clicked,[=](){
        if(link_mod==LINK_KAWASAKI)
        {
            QJsonObject json;
            json.insert(ASK_CAMERA_KEY_KAWASAKI, ASK_CAMERA_CLOSE_KAWASAKI);
            QString msg=JsonToQstring(json);
            client->write(msg.toUtf8());
            ui->record->append("发送:" + msg); // 将数据显示到记录框
        }
    });

    //写入任务号
    connect(ui->tasknumwriteBtn,&QPushButton::clicked,[=](){
        if(link_mod==LINK_KAWASAKI)
        {
            QString ID=ui->edit_tasknum->text();
            QJsonObject json;
            json.insert(ASK_TASKNUM_KEY_KAWASAKI, ID);
            QString msg=JsonToQstring(json);
            client->write(msg.toUtf8());
            ui->record->append("发送:" + msg); // 将数据显示到记录框
        }
    });

    //获取信息
    connect(ui->gitallinfoBtn,&QPushButton::clicked,[=](){
        if(link_mod==LINK_KAWASAKI)
        {
            QJsonObject json;
            json.insert(ASK_DELAY_KEY_KAWASAKI, ASK_DELAY_ONCE_KAWASAKI);
            json.insert(ASK_SEARCHSTAT_KEY_KAWASAKI, ASK_SEARCHSTAT_ONCE_KAWASAKI);
            json.insert(ASK_POS2_KEY_KAWASAKI, ASK_POS2_ONCE_KAWASAKI);
            json.insert(ASK_SIZE2_KEY_KAWASAKI, ASK_SIZE2_ONCE_KAWASAKI);
            QString msg=JsonToQstring(json);
            client->write(msg.toUtf8());
            ui->record->append("发送:" + msg); // 将数据显示到记录框
        }
    });

    //客户端接收数据
    connect(client,&QTcpSocket::readyRead,[=](){
        if(link_mod==LINK_NORMAL_ASCII||link_mod==LINK_KAWASAKI)
        {
            QByteArray artemp = "接收:";
            QByteArray array = client->readAll();
            ui->record->append(artemp+array);
            ReceiveMsg(array);
        }
        else if(link_mod==LINK_NORMAL_RTU)
        {
            QByteArray array = client->readAll();
            QString msg="接收:";
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
        else if(link_mod==LINK_KAWASAKI)
        {
            QJsonObject json=QstringToJson(msg);
            QJsonObject::Iterator it;
            for(it=json.begin();it!=json.end();it++)//遍历Key
            {
                QString keyString=it.key();
                if(keyString==ASE_LASER_KEY_KAWASAKI)//激光
                {
                    QString valueString=it.value().toString();
                    if(valueString==ASE_LASER_OPENOK_KAWASAKI)//激光打开成功
                    {
                        ui->record->append("解析:激光打开成功");
                    }
                    else if(valueString==ASE_LASER_OPENOK_KAWASAKI)//激光打开失败
                    {
                        ui->record->append("解析:激光打开失败");
                    }
                    else if(valueString==ASE_LASER_CLOSEOK_KAWASAKI)//激光关闭成功
                    {
                        ui->record->append("解析:激光关闭成功");
                    }
                    else if(valueString==ASE_LASER_CLOSEOK_KAWASAKI)//激光关闭失败
                    {
                        ui->record->append("解析:激光打开失败");
                    }
                }
                else if(keyString==ASE_CAMERA_KEY_KAWASAKI)//相机
                {
                    QString valueString=it.value().toString();
                    if(valueString==ASE_CAMERA_OPENOK_KAWASAKI)//相机打开成功
                    {
                        ui->record->append("解析:相机打开成功");
                    }
                    else if(valueString==ASE_CAMERA_OPENOK_KAWASAKI)//相机打开失败
                    {
                        ui->record->append("解析:相机打开失败");
                    }
                    else if(valueString==ASE_CAMERA_CLOSEOK_KAWASAKI)//相机关闭成功
                    {
                        ui->record->append("解析:相机关闭成功");
                    }
                    else if(valueString==ASE_CAMERA_CLOSEOK_KAWASAKI)//相机关闭失败
                    {
                        ui->record->append("解析:相机打开失败");
                    }
                }
                else if(keyString==ASE_TASKNUM_KEY_KAWASAKI)//设置任务号
                {
                    QString valueString=it.value().toString();
                    QStringList msgList = valueString.split(" ");
                    if(msgList.size()==2)
                    {
                         QString msg=" ";
                         msg=msg+msgList[1];
                         if(msg==ASE_TASKNUM_SETOK_KAWASAKI)
                         {
                             ui->record->append("解析:设置任务号"+msgList[0]+"成功");
                         }
                         else if(msg==ASE_TASKNUM_SETNG_KAWASAKI)
                         {
                             ui->record->append("解析:设置任务号"+msgList[0]+"失败");
                         }
                    }
                }
                else if(keyString==ASE_DELAY_KEY_KAWASAKI)//获取延迟
                {
                    QString valueString=it.value().toString();
                    if(valueString==ASE_DELAY_FAILED_KAWASAKI)//获取1次失败
                    {
                        ui->record->append("获取延迟失败");
                    }
                    else
                    {
                        ui->record->append("获取延迟:"+valueString);
                    }
                }
                else if(keyString==ASE_SEARCHSTAT_KEY_KAWASAKI)//获取搜索状态
                {
                    QString valueString=it.value().toString();
                    if(valueString==ASE_SEARCHSTAT_FAILED_KAWASAKI)//获取1次失败
                    {
                        ui->record->append("获取搜索状态失败");
                    }
                    else
                    {
                        ui->record->append("获取搜索状态:"+valueString);
                    }
                }
                else if(keyString==ASE_POS2_KEY_KAWASAKI)//获取二维坐标
                {
                    switch(it->type())
                    {
                        case QJsonValue::String:
                        {
                            QString valueString=it.value().toString();
                            if(valueString==ASE_POS2_FAILED_KAWASAKI)//获取1次失败
                            {
                                ui->record->append("获取二维坐标失败");
                            }
                        }
                        break;
                        case QJsonValue::Array:
                        {
                            QJsonArray versionArray=it.value().toArray();
                            QString msg="获取焊缝坐标pos2:Y=";
                            for(int i=0;i<versionArray.size();i++)
                            {
                                msg=msg+QString::number(versionArray[i].toDouble());
                                if(i<versionArray.size()-1)
                                {
                                    msg=msg+"mm,Z=";
                                }
                                else
                                {
                                    msg=msg+"mm";
                                }
                            }
                            ui->record->append(msg);
                        }
                        break;
                        default:
                        break;
                    }
                }
                else if(keyString==ASE_SIZE2_KEY_KAWASAKI)//获取二维尺寸
                {
                    switch(it->type())
                    {
                        case QJsonValue::String:
                        {
                            QString valueString=it.value().toString();
                            if(valueString==ASE_SIZE2_FAILED_KAWASAKI)//获取1次失败
                            {
                                ui->record->append("获取二维尺寸失败");
                            }
                        }
                        break;
                        case QJsonValue::Array:
                        {
                            QJsonArray versionArray=it.value().toArray();
                            QString msg="获取焊缝尺寸size2:宽度=";
                            for(int i=0;i<versionArray.size();i++)
                            {
                                msg=msg+QString::number(versionArray[i].toDouble());
                                if(i<versionArray.size()-1)
                                {
                                    msg=msg+"mm,高度=";
                                }
                                else
                                {
                                    msg=msg+"mm";
                                }
                            }
                            ui->record->append(msg);
                        }
                        break;
                        default:
                        break;
                    }
                }
                else if(keyString==ASE_POS6_KEY_KAWASAKI)//获取六维坐标
                {
                    switch(it->type())
                    {
                        case QJsonValue::String:
                        {
                            QString valueString=it.value().toString();
                            if(valueString==ASE_POS6_FAILED_KAWASAKI)//获取1次失败
                            {
                                ui->record->append("获取六维坐标失败");
                            }
                        }
                        break;
                        case QJsonValue::Array:
                        {
                            QJsonArray versionArray=it.value().toArray();
                            QString msg="解析:坐标pos6(";
                            for(int i=0;i<versionArray.size();i++)
                            {
                                msg=msg+QString::number(versionArray[i].toDouble());
                                if(i<versionArray.size()-1)
                                {
                                    msg=msg+",";
                                }
                                else
                                {
                                    msg=msg+")";
                                }
                            }
                            ui->record->append(msg);
                        }
                        break;
                        default:
                        break;
                    }
                }
            }
        }
    }
}

QJsonObject Client::QstringToJson(QString jsonString)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data());
    if(jsonDocument.isNull())
    {
        qDebug()<< "String NULL"<< jsonString.toLocal8Bit().data();
    }
    QJsonObject jsonObject = jsonDocument.object();
    return jsonObject;
}

QString Client::JsonToQstring(QJsonObject jsonObject)
{
    return QString(QJsonDocument(jsonObject).toJson());
}
