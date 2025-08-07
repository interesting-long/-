/*
 * Control.c
 *
 *  Created on: 2025��7��15��
 *      Author: interesting
 *
 *      ����������17
 *      ���Ʋ����б�
 *      ֱ�����PID:���KP          |���KD       | ֱ��KP       |ֱ��KD        |ֱ�����ֵ              5
 *      ���ٷ���   :���ٵ�KP        |���ٵ���ֵ                                                         2
 *      �ٶȲ���   :��ֱ���ж�ʱ��  |�����ж�ʱ��  |����ʱ��      |���ٵĴ�С     |�жϳ�ֱ����ʱ��        5
 *      ����ٶ�   :��·��PWM       | ��·��PWM                                                         2
 *      ����       :�����Ƿ���    |��ȡ������    |�˶�ʱ��                                             3
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
 *������������PID����
 * ����1���ṹ������ĵ�ַ
 * ����2����ǰֵ
 * ����3��Ŀ��ֵ
 * ���ӣ�Normal_PID(&servo_pid,MID_W-mid,0);//��servo��pid��������Ϊ�����߼�Ȩֵ������ֵ֮������ΪĿ��ֵ��0
 */
float Normal_PID(PID *PID,float NowData,float Point)
{

    PID->LastError=PID->Error;
    PID->Error = Point - NowData;
    PID->Out_D=PID->Error- PID->LastError;              //D��
    PID->Out_I+=PID->Error;                             //I��
    PID->Out_P=PID->Error;                              //P��
    return (PID->Kp * PID->Out_P + PID->Ki*PID->Out_I + PID->Kd * PID->Out_D);
}
/*
 *�����������pid����
 * ���ã������߼�Ȩֵ����pid�����趨Ϊָ������ֵ
 * ����1���趨������ֵ
 * ���ӣ�Servo_turn_pid(64);��Ŀ��ֵ����Ϊ64
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
 * ������ֱ�����PID
 * �������1����ȡ������ֵ
 * �������2�����з������ֵ
 * ����Ĭ��10-60��
 */
void Servo_Pid_Update(unsigned char mid,unsigned char yuzhi)
{
    unsigned int err;
    err=Distinguish_straight_Turn(Turn_Start,Turn_End);   //ƫ�����ж���ֱ��
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
 * �������ٶȲ���
 *
 */
void Motor_Update(void)
{
    if(abs(last_speed_time-speed_time)>long_time)//��ֱ������ʱ����ֵ�趨
    {
        flag=1;
    }
    if(speed_time>sp_time )                 //ֱ��������ʼʱ����
    {
        MotorR_SetSpeed(100*(MorR_Max + speed_add));
        MotorL_SetSpeed(100*(MorL_Max + speed_add));
        oled_show_string(0,7,"zhi   ");
    }
    else if(flag==1 && wan_time<=jian_time) //���ٲ��ԣ����ٵ�ʱ��20msΪһ֡����Ӧ2Ϊһ֡��jian_time���С��20
    {
        MotorR_SetSpeed(0*100);
        MotorL_SetSpeed(0*100);
    }
    else if(wan_time>jian_time)             //���ٺ���в���
    {
        flag=0;
        dif_speed(Final_mid_line,diff_yu,diff_kp);
        oled_show_string(0,7,"cha   ");
    }
    else                                    //�����ٶ�
    {
        flag=0;
        MotorR_SetSpeed(100*MorR_Max);
        MotorL_SetSpeed(100*MorL_Max);
        oled_show_string(0,7,"normal");
    }
}
/*
 * ���������籣��
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
/*�����������ٹ���
 * �������1����ȡ������ֵ
 * �������2���ж���ֵ
 * �������3�����ٵ����ֵ
 * �������4�����ٵ�Kp
 */
    void dif_speed(unsigned char mid,unsigned char yuzhi,float diff_Kp)
{
    unsigned int err;
//    err=(int)round(diff_Kp*(MID_W-mid));
//    err=Distinguish_straight_Turn(10,60);   //ƫ�����ж���ֱ��
    if(MID_W-mid>=0 && Fang_Error>=yuzhi)            //����
    {
        MotorR_SetSpeed(100*func_limit_ab(MorR_Max+5,20,45));
        MotorL_SetSpeed(100*func_limit_ab(MorL_Max-5,20,45));
    }
    else if(MID_W-mid<0 && Fang_Error>yuzhi)       //����
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
 * �������ж�ֱ���������ƫ����
 * �������1���ж�Э�������ʼ���������
 * �������2���ж�Э����Ľ������������
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
