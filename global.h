#ifndef GLOBAL_H
#define GLOBAL_H

#define MODBUS_REGISTERS_MAXNUM      400//125    //寄存器最大值//MODBUS_MAX_READ_REGISTERS每次连续读取最大值

#define MODBUS_ADD_POS      1           //坐标位置存储地址0x01
#define MODBUS_ADD_MOVETO   50          //移动位置存储地址0x32
#define MODBUS_ADD_LASER    257         //激光状态存储地址0x101
#define MODBUS_ADD_TASKNUM  258         //任务号存储地址0x102


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


#define ASK_LASER_KEY_KAWASAKI          "leaser"            //请求激光KEY
#define ASK_LASER_OPEN_KAWASAKI         "open"              //请求开激光
#define ASK_LASER_CLOSE_KAWASAKI        "close"             //请求关激光
#define ASE_LASER_KEY_KAWASAKI          "leaser"            //答复激光KEY
#define ASE_LASER_OPENOK_KAWASAKI       "open ok"           //开激光成功
#define ASE_LASER_OPENNG_KAWASAKI       "open ng"           //开激光失败
#define ASE_LASER_CLOSEOK_KAWASAKI      "close ok"          //关激光成功
#define ASE_LASER_CLOSENG_KAWASAKI      "close ng"          //关激光失败
#define ASK_CAMERA_KEY_KAWASAKI         "camera"            //请求相机KEY
#define ASK_CAMERA_OPEN_KAWASAKI        "open"              //请求开相机
#define ASK_CAMERA_CLOSE_KAWASAKI       "close"             //请求关相机
#define ASE_CAMERA_KEY_KAWASAKI         "camera"            //答复相机KEY
#define ASE_CAMERA_OPENOK_KAWASAKI      "open ok"           //开相机成功
#define ASE_CAMERA_OPENNG_KAWASAKI      "open ng"           //开相机失败
#define ASE_CAMERA_CLOSEOK_KAWASAKI     "close ok"          //关相机成功
#define ASE_CAMERA_CLOSENG_KAWASAKI     "close ng"          //关相机失败
#define ASK_TASKNUM_KEY_KAWASAKI        "tasknum"           //设置任务号KEY
#define ASE_TASKNUM_KEY_KAWASAKI        "tasknum"           //答复任务号KEY可
#define ASE_TASKNUM_SETOK_KAWASAKI      " ok"               //设置任务号成功
#define ASE_TASKNUM_SETNG_KAWASAKI      " ng"               //设置任务号失败
#define ASK_POS2_KEY_KAWASAKI           "getpos2"           //请求获取2维坐标KEY
#define ASK_POS2_ONCE_KAWASAKI          "once"              //请求获取2维坐标KEY一次
#define ASE_POS2_KEY_KAWASAKI           "getpos2"           //答复获取2维坐标KEY
#define ASE_POS2_FAILED_KAWASAKI        "failed"            //答复获取2维坐标KEY一次失败
#define ASK_POS6_KEY_KAWASAKI           "getpos6"           //请求获取6维坐标KEY
#define ASK_POS6_ONCE_KAWASAKI          "once"              //请求获取6维坐标KEY一次
#define ASE_POS6_KEY_KAWASAKI           "getpos6"           //答复获取6维坐标KEY
#define ASE_POS6_FAILED_KAWASAKI        "failed"            //答复获取6维坐标KEY一次失败


#endif // GLOBAL_H
