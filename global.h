#ifndef GLOBAL_H
#define GLOBAL_H

#define MODBUS_REGISTERS_MAXNUM      400//125    //寄存器最大值//MODBUS_MAX_READ_REGISTERS每次连续读取最大值

#define MODBUS_ADD_POS      30
#define MODBUS_ADD_MOVETO   50

#define ASK_GETPOS_ASCII        "GetPos:"   //请求坐标
#define ASE_GETPOS_ASCII        "Pos:"      //回复坐标
#define ASK_MOVETO_ASCII        "Moveto:"   //请求移动
#define ASE_MOVETO_ASCII        "Moved:"    //回复移动
#define ASK_MOVETOSP_ASCII      "speed:"    //请求移动速度
#define ASE_MOVETOSP_ASCII      "speed:"    //请求移动速度


#define RTU_MARK_ID_NUM         6               //RTU头文件包字符数
#define ASK_GETPOS_RTU          {1,2,3,4,5,6}   //请求坐标
#define ASE_GETPOS_RTU          {6,5,4,3,2,1}   //回复坐标
#define ASK_MOVETO_RTU          {3,2,1,1,2,3}   //请求移动
#define ASE_MOVETO_RTU          {3,2,1,3,2,1}   //回复移动

#endif // GLOBAL_H
