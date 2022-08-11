#include "server.h"
#include "ui_server.h"

#undef HAVE_ACCEPT4

Server::Server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);

    thread1 = new modbustcpThread(this);
    connect(thread1, SIGNAL(Send_show_registers_list()), this, SLOT(init_show_registers_list()));


    robot_mod=ROBOT_ZHICHANG;
    ui->comboBox->setCurrentIndex(robot_mod);

    link_mod=LINK_MODBUS_TCP;
    server_state=0;

    ui->radio_1->setChecked(1);

    //设置默认地址和端口
    ui->ip->setText("127.0.0.1");
    ui->port->setText(QString::number(1502)); // QString::number(9999)

    ui->edit_x->setText("10.101");
    ui->edit_y->setText("20.202");
    ui->edit_z->setText("30.303");
    ui->edit_rx->setText("40.404");
    ui->edit_ry->setText("50.505");
    ui->edit_rz->setText("60.606");

    ui->edit_delay->setText("100");
    ui->edit_searchstat->setText("0xff");
    ui->edit_weldY->setText("3003");
    ui->edit_weldZ->setText("4004");
    ui->edit_weldwidth->setText("5005");
    ui->edit_weldheight->setText("6006");


    ui->sendBtn->setEnabled(false);
//  ui->poswriteBtn->setEnabled(false);
//  ui->addwriteBtn->setEnabled(false);


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
    connect(ui->radio_4,&QRadioButton::clicked,[=](){
        link_mod=LINK_KAWASAKI;
    });
    connect(ui->radio_5,&QRadioButton::clicked,[=](){
        link_mod=LINK_CUSTOM;
    });

    mb_mapping = modbus_mapping_new(
        0, 0,
        MODBUS_REGISTERS_MAXNUM, 0);

    memset(mod_registers,0,sizeof(u_int16_t)*MODBUS_REGISTERS_MAXNUM);

    init_show_registers_list();

    //创建监听套接字
    connect(ui->startserverBtn,&QPushButton::clicked,[=](){
        if(link_mod==LINK_NORMAL_ASCII||link_mod==LINK_NORMAL_RTU||link_mod==LINK_KAWASAKI||link_mod==LINK_CUSTOM)
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
                        if(link_mod==LINK_NORMAL_ASCII||link_mod==LINK_KAWASAKI||link_mod==LINK_CUSTOM)
                        {
                            QByteArray artemp="接收:";
                            QByteArray array = conn->readAll();
                            QByteArray out_array;
                            ui->record->append(artemp+array);
                            ReceiveMsg(array,&out_array);
                        }
                        else if(link_mod==LINK_NORMAL_RTU)
                        {
                            QByteArray array = conn->readAll();
                            QByteArray out_array;
                            QString msg="接收:";
                            for(int n=0;n<array.size();n++)
                            {
                                uint8_t data=array[n];
                                msg=msg+"0x"+QString::number(data,16)+" ";
                            }
                            ui->record->append(msg);
                            ReceiveMsg(array,&out_array);
                        }
                    });

                });
                ui->startserverBtn->setText("关闭服务器");
                ui->radio_1->setEnabled(false);
                ui->radio_2->setEnabled(false);
                ui->radio_3->setEnabled(false);
                ui->radio_4->setEnabled(false);
                ui->sendBtn->setEnabled(true);
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
                ui->radio_4->setEnabled(true);
                ui->sendBtn->setEnabled(false);
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
                ui->radio_4->setEnabled(false);
                ui->sendBtn->setEnabled(true);
            }
            else
            {
                server_state=0;
                ui->startserverBtn->setText("启动服务器");
                ui->radio_1->setEnabled(true);
                ui->radio_2->setEnabled(true);
                ui->radio_3->setEnabled(true);
                ui->radio_4->setEnabled(true);
                ui->sendBtn->setEnabled(false);
            }
        }
    });

    connect(ui->sendBtn,&QPushButton::clicked,[=](){
        if(link_mod==LINK_NORMAL_ASCII||link_mod==LINK_KAWASAKI||link_mod==LINK_CUSTOM)
        {
            QString msg = ui->msgEdit->toPlainText();

            conn->write(msg.toUtf8()); // 发送数据

            ui->record->append("发送:" + msg); // 将数据显示到记录框
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
                ui->record->append("发送:" + showmsg); // 将数据显示到记录框
            }
        }
    });

    connect(ui->poswriteBtn,&QPushButton::clicked,[=](){
        float PosX=ui->edit_x->text().toFloat();
        float PosY=ui->edit_y->text().toFloat();
        float PosZ=ui->edit_z->text().toFloat();
        float PosRX=ui->edit_rx->text().toFloat();
        float PosRY=ui->edit_ry->text().toFloat();
        float PosRZ=ui->edit_rz->text().toFloat();
        QString msg;
        msg="("+ui->edit_x->text()+","+ui->edit_y->text()+","+ui->edit_z->text()+","+
                ui->edit_rx->text()+","+ui->edit_ry->text()+","+ui->edit_rz->text()+")";
        ui->record->append("Pos:" + msg); // 将数据显示到记录框

        float *freg=(float *)&mod_registers[MODBUS_ADD_POS];
        freg[0]=PosX;
        freg[1]=PosY;
        freg[2]=PosZ;
        freg[3]=PosRX;
        freg[4]=PosRY;
        freg[5]=PosRZ;
        init_show_registers_list();

        if(link_mod==LINK_NORMAL_ASCII||link_mod==LINK_NORMAL_RTU||link_mod==LINK_KAWASAKI||link_mod==LINK_CUSTOM)
        {

        }
        else if(link_mod==LINK_MODBUS_TCP)
        {
            float *f_tab_reg=(float *)&mb_mapping->tab_registers[MODBUS_ADD_POS];
            f_tab_reg[0]=PosX;
            f_tab_reg[1]=PosY;
            f_tab_reg[2]=PosZ;
            f_tab_reg[3]=PosRX;
            f_tab_reg[4]=PosRY;
            f_tab_reg[5]=PosRZ;
        }
    });

    connect(ui->poswriteBtn_2,&QPushButton::clicked,[=](){
        std::string strdata;
        u_int16_t data_delay,data_searchstat,data_Weld_Y,data_Weld_Z,data_Weld_W,data_Weld_H;
        u_int16_t *u16_reg=(u_int16_t *)mod_registers;
        QString msg;

        strdata=ui->edit_delay->text().toStdString();
        data_delay=std::stoi(strdata,NULL,0);
        strdata=ui->edit_searchstat->text().toStdString();
        data_searchstat=std::stoi(strdata,NULL,0);
        strdata=ui->edit_weldY->text().toStdString();
        data_Weld_Y=std::stoi(strdata,NULL,0);
        strdata=ui->edit_weldZ->text().toStdString();
        data_Weld_Z=std::stoi(strdata,NULL,0);
        strdata=ui->edit_weldwidth->text().toStdString();
        data_Weld_W=std::stoi(strdata,NULL,0);
        strdata=ui->edit_weldheight->text().toStdString();
        data_Weld_H=std::stoi(strdata,NULL,0);
        if(robot_mod==ROBOT_ZHICHANG)
        {
            u16_reg[MODBUS_ADD_DELAY]=data_delay;
            u16_reg[MODBUS_ADD_SEARCHSTAT]=data_searchstat;
            u16_reg[MODBUS_ADD_WELD_Y_POS]=data_Weld_Y;
            u16_reg[MODBUS_ADD_WELD_Z_POS]=data_Weld_Z;
            u16_reg[MODBUS_ADD_WELD_W_SIZE]=data_Weld_W;
            u16_reg[MODBUS_ADD_WELD_H_SIZE]=data_Weld_H;
            mb_mapping->tab_registers[MODBUS_ADD_DELAY]=data_delay;
            mb_mapping->tab_registers[MODBUS_ADD_SEARCHSTAT]=data_searchstat;
            mb_mapping->tab_registers[MODBUS_ADD_WELD_Y_POS]=data_Weld_Y;
            mb_mapping->tab_registers[MODBUS_ADD_WELD_Z_POS]=data_Weld_Z;
            mb_mapping->tab_registers[MODBUS_ADD_WELD_W_SIZE]=data_Weld_W;
            mb_mapping->tab_registers[MODBUS_ADD_WELD_H_SIZE]=data_Weld_H;
        }
        else if(robot_mod==ROBOT_NABOTE)
        {
            u16_reg[MODBUS_NABO_SEARCHSTAT]=data_searchstat;
            u16_reg[MODBUS_NABO_WELD_Y_POS]=data_Weld_Y;
            u16_reg[MODBUS_NABO_WELD_Z_POS]=data_Weld_Z;
            mb_mapping->tab_registers[MODBUS_NABO_SEARCHSTAT]=data_searchstat;
            mb_mapping->tab_registers[MODBUS_NABO_WELD_Y_POS]=data_Weld_Y;
            mb_mapping->tab_registers[MODBUS_NABO_WELD_Z_POS]=data_Weld_Z;
        }

        msg="设置:\n延迟:"+QString::number(data_delay)+"ms\n搜索状态:"
                +QString::number(data_searchstat)+"\n焊缝坐标Y:"
                +QString::number(data_Weld_Y/100.0)+"mm\n焊缝坐标Z:"
                +QString::number(data_Weld_Z/100.0)+"mm\n焊缝宽度:"
                +QString::number(data_Weld_W/100.0)+"mm\n焊缝高度:"
                +QString::number(data_Weld_H/100.0)+"mm";
        ui->record->append(msg); // 将数据显示到记录框
        init_show_registers_list();
    });

    connect(ui->addwriteBtn,&QPushButton::clicked,[=](){

        std::string stradd=ui->edit_addwrite->text().toStdString();
        std::string strdata=ui->edit_datawrite->text().toStdString();
        int addr=std::stoi(stradd,NULL,0);
        uint16_t data=std::stoi(strdata,NULL,0);
        if(addr>=0&&addr<MODBUS_REGISTERS_MAXNUM)
        {
            mod_registers[addr]=data;
        }
        mb_mapping->tab_registers[addr]=data;
        init_show_registers_list();
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

void Server::init_show_registers_list()
{
    ui->record_2->clear();
    for(int n=0;n<MODBUS_REGISTERS_MAXNUM;n++)
    {
        QString msg=QString::number(n,16);
        QString data=QString::number(mod_registers[n],16);
        ui->record_2->append("ChangeRegister:[0x"+msg+"]: 0x"+data);
    }
}

void Server::ReceiveMsg(QByteArray array,QByteArray *sent_array)
{
    QByteArray outarray;

    if(link_mod==LINK_NORMAL_ASCII)
    {
        /***********************/
        //这里读数据
        if(array==ASK_GETPOS_ASCII)//获取坐标
        {
            float *freg=(float *)&mod_registers[MODBUS_ADD_POS];
            QString msg;
            QString msgNull=" ";
            msg=ASE_GETPOS_ASCII+msgNull+QString::number(freg[0])+" "+QString::number(freg[1])+" "+
                        QString::number(freg[2])+" "+QString::number(freg[3])+" "+
                        QString::number(freg[4])+" "+QString::number(freg[5]);

            conn->write(msg.toUtf8());
        }
    //  else if(array==)//其他信息
    //  {

    //  }

        /************************/
        //这里写数据
        else
        {
            QString rcvmsg=array;
            int pos=rcvmsg.indexOf(":");
            if(pos>=0)
            {
                QString rcvmsgId=rcvmsg.left(pos+1);
                if(rcvmsgId==ASK_MOVETO_ASCII)//获取坐标
                {
                    float PosX,PosY,PosZ,PosRX,PosRY,PosRZ,PosSP;
                    QString strpos=rcvmsg;
                    strpos.chop(strpos.size()-strpos.lastIndexOf(")")); //chop把字符串后N个丢弃,chop是直接对对象进行操作(会改变对象)
                    strpos=strpos.right(strpos.size()-strpos.indexOf("(")-1);
                    QStringList posList = strpos.split(",");
                    if(posList.size()!=6)
                    {
                        fprintf(stderr, "msg is too short\n",NULL);
                    }
                    else
                    {
                        PosX=posList[0].toFloat();
                        PosY=posList[1].toFloat();
                        PosZ=posList[2].toFloat();
                        PosRX=posList[3].toFloat();
                        PosRY=posList[4].toFloat();
                        PosRZ=posList[5].toFloat();

                        float *freg=(float *)&mod_registers[MODBUS_ADD_MOVETO];
                        freg[0]=PosX;
                        freg[1]=PosY;
                        freg[2]=PosZ;
                        freg[3]=PosRX;
                        freg[4]=PosRY;
                        freg[5]=PosRZ;
                    }

                    QString speed=rcvmsg;
                    int epos = rcvmsg.lastIndexOf(")");//从后面开始查找
                    speed=rcvmsg.right(rcvmsg.size()-epos-1-1);//输出右边第N个字符串"speed:xxx"
                    int pos=speed.indexOf(":");
                    if(pos>=0)
                    {
                        QString rcvmsg2Id=speed.left(pos+1);//"speed:"
                        if(rcvmsg2Id==ASK_MOVETOSP_ASCII)
                        {
                            QString strspeed=speed.right(speed.size()-pos-1);
                            PosSP=strspeed.toFloat(); 
                            float *freg=(float *)&mod_registers[MODBUS_ADD_MOVETO];
                            freg[6]=PosSP;
                        }
                    //  else if(array==)//其他速度信息
                    //  {

                    //  }
                    }
                    init_show_registers_list();
                }
            //  else if(rcvmsgId==)//其他信息
            //  {

            //  }
            }
        }
    }
    else if(link_mod==LINK_NORMAL_RTU)
    {
        /********************/
        //这里读数据
        const char cart[] = ASK_GETPOS_RTU;
        QByteArray recv_group(QByteArray::fromRawData(cart, sizeof(cart)));
        if(array==recv_group)//获取坐标
        {
            uint8_t *u8reg=(uint8_t *)&mod_registers[MODBUS_ADD_POS];
            const char cartsent[] = ASE_GETPOS_RTU;
            QByteArray sent_group(QByteArray::fromRawData(cartsent, sizeof(cartsent)));
            QByteArray sent_group2(QByteArray::fromRawData((char*)u8reg, 6*sizeof(float)));
            sent_group=sent_group+sent_group2;
            conn->write(sent_group);
        } 
    //  else if(array==)
    //  {

    //  }
        else
        {
            /********************/
            //这里写数据
            char rcvId[RTU_MARK_ID_NUM];
            strncpy(rcvId, array.data(),RTU_MARK_ID_NUM);
            QByteArray recv_Id(QByteArray::fromRawData(rcvId, sizeof(rcvId)));

            const char move[] = ASK_MOVETO_RTU;
            QByteArray recv_move(QByteArray::fromRawData(move, sizeof(move)));
            if(recv_Id==recv_move)//电机移动
            {
                if(array.size()<RTU_MARK_ID_NUM+7*sizeof(float))
                {
                    fprintf(stderr, "msg is too short\n",NULL);
                }
                else
                {
                    uint8_t *u8reg=(uint8_t *)&mod_registers[MODBUS_ADD_MOVETO];
                    memcpy(u8reg,array.data()+RTU_MARK_ID_NUM,7*sizeof(float));
                    init_show_registers_list();
                    const char moved[] = ASE_MOVETO_RTU;
                    QByteArray sent_group(QByteArray::fromRawData(moved, sizeof(moved)));
                    conn->write(sent_group);
                    float *f_reg=(float*)u8reg;
                    QString my_msg;
                    my_msg="(移动指令:坐标["+QString::number(f_reg[0])+","+QString::number(f_reg[1])+","+
                            QString::number(f_reg[2])+","+QString::number(f_reg[3])+","+
                            QString::number(f_reg[4])+","+QString::number(f_reg[5])+"] 速度:"+
                            QString::number(f_reg[6])+")";
                    ui->record->append(my_msg);
                }
            }
        }
    }
    else if(link_mod==LINK_KAWASAKI)
    {
        /***********************/
        //这里写数据
    #ifdef USE_PARENTHESES_INSTEAD_QUOTATION
        for(unsigned int n=0;n<array.size();n++)
        {
            if(array[n]=='('||array[n]==')')
            {
               array[n]='"';
            }
        }
    #endif
        QJsonObject json=QstringToJson(array);
        QJsonObject::Iterator it;

        QJsonObject anser;
        for(it=json.begin();it!=json.end();it++)//遍历Key
        {
            QString keyString=it.key();
            if(keyString==ASK_LASER_KEY_KAWASAKI)//激光
            {
                QString valueString=it.value().toString();
                if(valueString==ASK_LASER_OPEN_KAWASAKI)//请求开激光
                {
                    if(robot_mod==ROBOT_ZHICHANG)
                    {
                        mod_registers[MODBUS_ADD_LASER]=1;
                    }
                    else
                    {
                        mod_registers[MODBUS_NABO_LASER]=1;
                    }
                    anser.insert(ASE_LASER_KEY_KAWASAKI, ASE_LASER_OPENOK_KAWASAKI);
                    init_show_registers_list();
                    ui->record->append("解析:请求开激光"); // 将数据显示到记录框
                }
                else if(valueString==ASK_LASER_CLOSE_KAWASAKI)//请求关激光
                {
                    if(robot_mod==ROBOT_ZHICHANG)
                    {
                        mod_registers[MODBUS_ADD_LASER]=0;
                    }
                    else
                    {
                        mod_registers[MODBUS_NABO_LASER]=0;
                    }
                    anser.insert(ASE_LASER_KEY_KAWASAKI, ASE_LASER_CLOSEOK_KAWASAKI);
                    init_show_registers_list();
                    ui->record->append("解析:请求关激光");
                }
            }
            else if(keyString==ASK_CAMERA_KEY_KAWASAKI)//相机
            {
                QString valueString=it.value().toString();
                if(valueString==ASK_CAMERA_OPEN_KAWASAKI)//请求开相机
                {
                    if(robot_mod==ROBOT_ZHICHANG)
                    {
                        mod_registers[MODBUS_ADD_LASER]=1;
                    }
                    else
                    {
                        mod_registers[MODBUS_NABO_WELDING]=1;
                    }
                    anser.insert(ASE_CAMERA_KEY_KAWASAKI, ASE_CAMERA_OPENOK_KAWASAKI);
                    init_show_registers_list();
                    ui->record->append("解析:请求开相机"); // 将数据显示到记录框
                }
                else if(valueString==ASK_CAMERA_CLOSE_KAWASAKI)//请求关相机
                {
                    if(robot_mod==ROBOT_ZHICHANG)
                    {
                        mod_registers[MODBUS_ADD_LASER]=0;
                    }
                    else
                    {
                        mod_registers[MODBUS_NABO_WELDING]=0;
                    }
                    anser.insert(ASE_CAMERA_KEY_KAWASAKI, ASE_CAMERA_CLOSEOK_KAWASAKI);
                    init_show_registers_list();
                    ui->record->append("解析:请求关相机");
                }
            }
            else if(keyString==ASK_TASKNUM_KEY_KAWASAKI)//任务号
            {
                QString valueString=it.value().toString();
                int ID=valueString.toInt();
                if(robot_mod==ROBOT_ZHICHANG)
                {
                    mod_registers[MODBUS_ADD_TASKNUM]=ID;
                }
                else
                {
                    mod_registers[MODBUS_NABO_TASKNUM]=ID;
                }
                anser.insert(ASE_TASKNUM_KEY_KAWASAKI, valueString+ASE_TASKNUM_SETOK_KAWASAKI);
                init_show_registers_list();
                ui->record->append("解析:设置任务号:"+valueString);
            }
            else if(keyString==ASK_DELAY_KEY_KAWASAKI)//获取焊缝延迟
            {
                switch(it->type())
                {
                    case QJsonValue::String:
                    {
                        QString valueString=it.value().toString();
                        if(valueString==ASK_DELAY_ONCE_KAWASAKI)//获取1次
                        {
                            u_int16_t u16_data=mod_registers[MODBUS_ADD_DELAY];
                            QString msg;
                            if(ui->checkBox_daley->isChecked()==true)
                            {
                                msg=ASE_DELAY_FAILED_KAWASAKI;
                            }
                            else
                            {
                                msg=QString::number(u16_data);
                            }
                            anser.insert(ASE_DELAY_KEY_KAWASAKI, msg);
                            ui->record->append("解析:请求获取1次延迟");
                        }
                    }
                    break;
                    default:
                    break;
                 }
            }
            else if(keyString==ASK_SEARCHSTAT_KEY_KAWASAKI)//获取搜索状态
            {
                switch(it->type())
                {
                    case QJsonValue::String:
                    {
                        QString valueString=it.value().toString();
                        if(valueString==ASK_SEARCHSTAT_ONCE_KAWASAKI)//获取1次
                        {
                            u_int16_t u16_data=mod_registers[MODBUS_ADD_SEARCHSTAT];
                            if(robot_mod==ROBOT_ZHICHANG)
                            {
                                u16_data=mod_registers[MODBUS_ADD_SEARCHSTAT];
                            }
                            else if(robot_mod==ROBOT_NABOTE)
                            {
                                u16_data=mod_registers[MODBUS_NABO_SEARCHSTAT];
                            }
                            QString msg="ng";
                            if(ui->checkBox_searchstat->isChecked()==true)
                            {
                                msg=ASE_SEARCHSTAT_FAILED_KAWASAKI;
                            }
                            else
                            {
                                if(u16_data==0)
                                {
                                    msg="ng";
                                }
                                else if(u16_data==255)
                                {
                                    msg="ok";
                                }
                            }
                            anser.insert(ASE_SEARCHSTAT_KEY_KAWASAKI, msg);
                            ui->record->append("解析:请求获取1次搜索状态");
                        }
                    }
                    break;
                    default:
                    break;
                 }
            }
            else if(keyString==ASK_POS2_KEY_KAWASAKI)//获取2维坐标
            {
                switch(it->type())
                {
                    case QJsonValue::String:
                    {
                        QString valueString=it.value().toString();
                        if(valueString==ASK_POS2_ONCE_KAWASAKI)//获取1次
                        { 
                            int16_t i16_y=mod_registers[MODBUS_ADD_WELD_Y_POS],i16_z=mod_registers[MODBUS_ADD_WELD_Z_POS];
                            if(robot_mod==ROBOT_ZHICHANG)
                            {
                                i16_y=mod_registers[MODBUS_ADD_WELD_Y_POS];
                                i16_z=mod_registers[MODBUS_ADD_WELD_Z_POS];
                            }
                            else if(robot_mod==ROBOT_NABOTE)
                            {
                                i16_y=mod_registers[MODBUS_NABO_WELD_Y_POS];
                                i16_z=mod_registers[MODBUS_NABO_WELD_Z_POS];
                            }

                            float f16_weld_Y=i16_y/100.0;
                            float f16_weld_Z=i16_z/100.0;
                            QJsonArray versionArray;
                            if(ui->checkBox_pos2->isChecked()==true)
                            {
                                anser.insert(ASE_POS2_KEY_KAWASAKI, ASE_POS2_FAILED_KAWASAKI);
                            }
                            else
                            {
                                versionArray.append(f16_weld_Y);
                                versionArray.append(f16_weld_Z);
                                anser.insert(ASE_POS2_KEY_KAWASAKI, QJsonValue(versionArray));
                            }
                            ui->record->append("解析:请求获取1次2维坐标");
                        }
                    }
                    break;
                    default:
                    break;
                 }
            }
            else if(keyString==ASK_SIZE2_KEY_KAWASAKI)//获取焊缝宽高
            {
                switch(it->type())
                {
                    case QJsonValue::String:
                    {
                        QString valueString=it.value().toString();
                        if(valueString==ASK_SIZE2_ONCE_KAWASAKI)//获取1次
                        {
                            float f16_weld_W=mod_registers[MODBUS_ADD_WELD_W_SIZE]/100.0;
                            float f16_weld_H=mod_registers[MODBUS_ADD_WELD_H_SIZE]/100.0;
                            QJsonArray versionArray;
                            if(ui->checkBox_size2->isChecked()==true)
                            {
                                 anser.insert(ASE_SIZE2_KEY_KAWASAKI, ASE_SIZE2_FAILED_KAWASAKI);
                            }
                            else
                            {
                                versionArray.append(f16_weld_W);
                                versionArray.append(f16_weld_H);
                                anser.insert(ASE_SIZE2_KEY_KAWASAKI, QJsonValue(versionArray));
                            }
                            ui->record->append("解析:请求获取1次2维尺寸");
                        }
                    }
                    break;
                    default:
                    break;
                 }
            }
            else if(keyString==ASK_POS6_KEY_KAWASAKI)//获取6维坐标
            {
                switch(it->type())
                {
                    case QJsonValue::String:
                    {
                        QString valueString=it.value().toString();
                        if(valueString==ASK_POS6_ONCE_KAWASAKI)//获取1次
                        {
                            float *freg=(float *)&mod_registers[MODBUS_ADD_POS];
                            QJsonArray versionArray;
                            for(int i=0;i<6;i++)
                            {
                                versionArray.append(freg[i]);
                            }
                            anser.insert(ASE_POS6_KEY_KAWASAKI, QJsonValue(versionArray));
                            ui->record->append("解析:请求获取1次6维坐标");
                        }
                    }
                    break;
                    default:
                    break;
                 }
            }
        }
        if(anser.size()!=0)
        {
            QString msg=JsonToQstring(anser);
        #ifdef USE_PARENTHESES_INSTEAD_QUOTATION
            int time_s=0;
            for(unsigned int n=0;n<msg.size();n++)
            {
                if(msg[n]=='"')   //"
                {
                    if(time_s==0)
                    {
                        msg[n]='('; //(
                        time_s=1;
                    }
                    else if(time_s==1)
                    {
                        msg[n]=')'; //)
                        time_s=0;
                    }
                }
            }
        #endif
        #ifdef DEL_SPACE_AND_LINEN
            QString msg2;
            for(unsigned int n=0;n<msg.size();n++)
            {
                if(msg[n]!='\n')
                {
                    msg2.push_back(msg[n]);
                }
            }
            msg=msg2;
        #endif
            conn->write(msg.toUtf8());
            ui->record->append("发送:" + msg); // 将数据显示到记录框
        }
    }
    else if(link_mod==LINK_CUSTOM)
    {

    }

    *sent_array=outarray;
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
        //_p->ctx = modbus_new_tcp(server_ip.toUtf8(), server_port.toInt());
        _p->ctx = modbus_new_tcp(NULL, server_port.toInt());
        _p->sock = modbus_tcp_listen(_p->ctx, 1);//最大监听1路
        modbus_tcp_accept(_p->ctx, &_p->sock);
        while(_p->server_state==1)
        {
            int rc;
            uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
            rc = modbus_receive(_p->ctx, query);
            if (rc > 0) {
              modbus_reply(_p->ctx, query, rc, _p->mb_mapping);
              for(int n=0;n<MODBUS_REGISTERS_MAXNUM;n++)
              {
                if(_p->mb_mapping->tab_registers[n]!=_p->mod_registers[n])
                {
                    _p->mod_registers[n]=_p->mb_mapping->tab_registers[n];
                    QString msg=QString::number(n,16);
                    QString data=QString::number(_p->mb_mapping->tab_registers[n],16);
                    _p->ui->record->append("ChangeRegister:[0x"+msg+"]: "+data);
                }
              }
            // _p->init_show_registers_list();
              emit Send_show_registers_list();
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

QJsonObject Server::QstringToJson(QString jsonString)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data());
    if(jsonDocument.isNull())
    {
        qDebug()<< "String NULL"<< jsonString.toLocal8Bit().data();
    }
    QJsonObject jsonObject = jsonDocument.object();
    return jsonObject;
}

QString Server::JsonToQstring(QJsonObject jsonObject)
{
    return QString(QJsonDocument(jsonObject).toJson());
}

void Server::on_comboBox_currentIndexChanged(int index)
{
    robot_mod=index;
    if(robot_mod==ROBOT_ZHICHANG)
    {
        ui->edit_searchstat->setText("0xff");
    }
    else if(robot_mod==ROBOT_NABOTE)
    {
        ui->edit_searchstat->setText("0x01");
    }
}

