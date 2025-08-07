/*
 * Motor.c
 *
 *  Created on: 2025年7月15日
 *      Author: interesting
 */

#include "Motor.h"
/*
 * 函数功能：电机的初始化
 * 函数名称：Motor_Init();
 * 输入参数：无
 * 输出参数：无
 */
void Motor_Init(void)
{
    pwm_init(MotorL_pwm2,17000,0);
    pwm_init(MotorL_pwm1,17000,0);
    pwm_init(MotorR_pwm2,17000,0);
    pwm_init(MotorR_pwm1,17000,0);
}
/*
 * 函数功能：左路电机PWM设置
 * 函数名称：MotorL_SetSpeed(uint8);
 * 输入参数：想要的PWM值
 * 输出参数：无
 */
void MotorL_SetSpeed(int pwm)
{
    if(pwm>=0)
    {
        pwm_set_duty(MotorL_pwm2,0);
        pwm_set_duty(MotorL_pwm1,pwm);
    }
    else
    {
        pwm_set_duty(MotorL_pwm2,pwm);
        pwm_set_duty(MotorL_pwm1,0);
    }
}
/*
 * 函数功能：右路电机PWM设置
 * 函数名称：MotorR_SetSpeed(uint8);
 * 输入参数：想要的PWM值
 * 输出参数：无
 */
void MotorR_SetSpeed(int pwm)
{
    if(pwm>0)
    {
        pwm_set_duty(MotorR_pwm2,0);
        pwm_set_duty(MotorR_pwm1,pwm);
    }
    else
    {
        pwm_set_duty(MotorR_pwm2,pwm);
        pwm_set_duty(MotorR_pwm1,0);
    }
}
