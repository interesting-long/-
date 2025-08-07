/*
 * Control.h
 *
 *  Created on: 2025年7月15日
 *      Author: interesting
 */

#ifndef CODE_CONTROL_H_
#define CODE_CONTROL_H_

typedef struct PID
{
        float Kp;
        float Ki;
        float Kd;
        float LowPass;

        float Out_P;
        float Out_I;
        float Out_D;

        float PrevError;
        float LastError;
        float Error;
        float LastData;
}PID;
#define PID_CREATE(_kp,_ki,_kd,_low_pass) \
{ \
    .Kp=_kp, \
    .Ki=_ki, \
    .Kd=_kd, \
    .LowPass=_low_pass, \
    .Out_P=0, \
    .Out_I=0, \
    .Out_D=0, \
}
#include "zf_common_headfile.h"

#define duoji_mid 820   //舵机的中间值
#define Turn_Start 10   //判断直弯的起始行
#define Turn_End    60  //判断直弯的终止行
extern int speed_time;
extern int wan_time;
extern int last_speed_time;
extern int Fang_Error;
extern PID servo_pid;

void Servo_Pid_Update(unsigned char mid,unsigned char yuzhi);
void Servo_turn_pid(unsigned char mid);
void Time_Stop_ALL_Control(void);
void Over_Stop_ALL_Control(void);
void Motor_Update(void);
void dif_speed(unsigned char mid,unsigned char yuzhi,float diff_Kp);
unsigned char Protect_control(void);
int Distinguish_straight_Turn(unsigned char start,unsigned char end);

#endif /* CODE_CONTROL_H_ */
