/*
 * Control.c
 *
 *  Created on: 2025年7月15日
 *      Author: interesting
 *
 *      参数个数：17
 *      控制参数列表
 *      直弯分离PID:弯道KP          |弯道KD       | 直道KP       |直道KD        |直弯的阈值              5
 *      差速分离   :差速的KP        |差速的阈值                                                         2
 *      速度策略   :长直道判断时间  |加速判断时间  |减速时间      |加速的大小     |判断长直道的时间        5
 *      电机速度   :左路的PWM       | 右路的PWM                                                         2
 *      杂项       :保护是否开启    |读取的行数    |运动时间                                             3
 *
 */

#include "Control.h"
PID servo_pid;
int speed_time;
int last_speed_time;
int wan_time;
int Fang_Error;
unsigned char flag;
/*
 *函数：常见的PID计算
 * 参数1：结构体变量的地址
 * 参数2：当前值
 * 参数3：目标值
 * 例子：Normal_PID(&servo_pid,MID_W-mid,0);//将servo的pid参数设置为以中线加权值和中线值之差设置为目标值：0
 */
float Normal_PID(PID *PID,float NowData,float Point)
{

    PID->LastError=PID->Error;
    PID->Error = Point - NowData;
    PID->Out_D=PID->Error- PID->LastError;              //D项
    PID->Out_I+=PID->Error;                             //I项
    PID->Out_P=PID->Error;                              //P项
    return (PID->Kp * PID->Out_P + PID->Ki*PID->Out_I + PID->Kd * PID->Out_D);
}
/*
 *函数：舵机的pid控制
 * 作用：将中线加权值利用pid控制设定为指定中线值
 * 参数1：设定的中线值
 * 例子：Servo_turn_pid(64);将目标值设置为64
 */
void Servo_turn_pid(unsigned char mid)
{
    int servo_value;
    int temp;
    servo_value=Normal_PID(&servo_pid,MID_W-mid,0);
    temp=func_limit_ab(servo_value,-59,60);
    Ser_Servo_Duty(duoji_mid-temp);
}
/*
 * 函数：直弯分离PID
 * 传入参数1：读取的中线值
 * 传入参数2：进行分离的阈值
 * 方差默认10-60行
 */
void Servo_Pid_Update(unsigned char mid,unsigned char yuzhi)
{
    unsigned int err;
    err=Distinguish_straight_Turn(Turn_Start,Turn_End);   //偏方差判断弯直道
//    err=abs(mid_line_list2[mid]-MID_W);
    if(err>=yuzhi)
    {
        servo_pid.Kp = ser_Max_KP;
        servo_pid.Ki = 0;
        servo_pid.Kd = ser_Max_KD;
    }
    else
    {
        servo_pid.Kp = ser_Min_KP;
        servo_pid.Ki = 0;
        servo_pid.Kd = ser_Min_KD;
    }
}

void Time_Stop_ALL_Control(void)
{
    oled_clear();
    pit_disable(CCU60_CH1);
    MotorL_SetSpeed(0);
    MotorR_SetSpeed(0);
    Ser_Servo_Duty(duoji_mid);
    oled_show_string(0,4,"TIME!");
    while(Show_image_mode)
    {}
}
void Over_Stop_ALL_Control(void)
{
    oled_clear();
    pit_disable(CCU60_CH1);
    MotorL_SetSpeed(0);
    MotorR_SetSpeed(0);
    Ser_Servo_Duty(duoji_mid);
    oled_show_string(0,4,"OVER!");
    while(Show_image_mode)
    {}
}
/*
 * 函数：速度策略
 *
 */
void Motor_Update(void)
{
    if(abs(last_speed_time-speed_time)>long_time)//长直道加速时间阈值设定
    {
        flag=1;
    }
    if(speed_time>sp_time )                 //直道加速起始时间间隔
    {
        MotorR_SetSpeed(100*(MorR_Max + speed_add));
        MotorL_SetSpeed(100*(MorL_Max + speed_add));
        oled_show_string(0,7,"zhi   ");
    }
    else if(flag==1 && wan_time<=jian_time) //减速策略：减速的时间20ms为一帧，对应2为一帧，jian_time最好小于20
    {
        MotorR_SetSpeed(0*100);
        MotorL_SetSpeed(0*100);
    }
    else if(wan_time>jian_time)             //减速后进行差速
    {
        flag=0;
        dif_speed(Final_mid_line,diff_yu,diff_kp);
        oled_show_string(0,7,"cha   ");
    }
    else                                    //正常速度
    {
        flag=0;
        MotorR_SetSpeed(100*MorR_Max);
        MotorL_SetSpeed(100*MorL_Max);
        oled_show_string(0,7,"normal");
    }
}
/*
 * 函数：出界保护
 */
unsigned char Protect_control(void)
{
    if(     image[59][0]!=IMG_BLACK             ||
                            image[59][MT9V03X_W-1]!=IMG_BLACK   ||
                            image[69][0]!=IMG_BLACK             ||
                            image[69][MT9V03X_W*1/5]!=IMG_BLACK ||
                            image[69][MT9V03X_W*2/5]!=IMG_BLACK ||
                            image[69][MT9V03X_W*3/5]!=IMG_BLACK ||
                            image[69][MT9V03X_W*4/5]!=IMG_BLACK ||
                            image[69][MT9V03X_W-1]!=IMG_BLACK   ||
                            Protect<=10)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/*函数名：差速过弯
 * 传入参数1：读取的中线值
 * 传入参数2：判断阈值
 * 传入参数3：差速的最大值
 * 传入参数4：差速的Kp
 */
    void dif_speed(unsigned char mid,unsigned char yuzhi,float diff_Kp)
{
    unsigned int err;
//    err=(int)round(diff_Kp*(MID_W-mid));
//    err=Distinguish_straight_Turn(10,60);   //偏方差判断弯直道
    if(MID_W-mid>=0 && Fang_Error>=yuzhi)            //左弯
    {
        MotorR_SetSpeed(100*func_limit_ab(MorR_Max+5,20,45));
        MotorL_SetSpeed(100*func_limit_ab(MorL_Max-5,20,45));
    }
    else if(MID_W-mid<0 && Fang_Error>yuzhi)       //右弯
    {
        MotorL_SetSpeed(100*func_limit_ab(MorL_Max+5,20,45));
        MotorR_SetSpeed(100*func_limit_ab(MorR_Max-5,20,45));
    }
//    if(MID_W-mid>=yuzhi)
//    {
//        MotorR_SetSpeed(100*func_limit_ab(MorR_Max+err,3,80));
//        MotorL_SetSpeed(100*func_limit_ab(MorL_Max-err,3,80));
//    }
//    else if(MID_W-mid<=-yuzhi)
//    {
//        MotorL_SetSpeed(100*func_limit_ab(MorL_Max+(-err),3,80));
//        MotorR_SetSpeed(100*func_limit_ab(MorR_Max+err,3,80));
//    }
//    else
//    {
//        MotorR_SetSpeed(100*MorR_Max);
//        MotorL_SetSpeed(100*MorL_Max);
//    }
}
/*
 * 函数：判断直道与弯道的偏方差
 * 传入参数1：判断协方差的起始点的纵坐标
 * 传入参数2：判断协方差的结束点的纵坐标
 */
int Distinguish_straight_Turn(unsigned char start, unsigned char end)
{
    unsigned char temp;
    int sum = 0;
    if (start > end)
    {
        temp = end;
        end = start;
        start = temp;
    }
    unsigned char n = end - start + 1;
    cross_buxian(mid_line_list2[start], mid_line_list2[end], start, end, vari);
    for (unsigned char i = start+1; i < end; i++)
    {
        sum += (mid_line_list2[i] - vari_line_list[i]) * (mid_line_list2[i] - vari_line_list[i]);
    }
    return (int)((float)sum / n + 0.5f);
}
