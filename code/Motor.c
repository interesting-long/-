/*
 * Motor.c
 *
 *  Created on: 2025��7��15��
 *      Author: interesting
 */

#include "Motor.h"
/*
 * �������ܣ�����ĳ�ʼ��
 * �������ƣ�Motor_Init();
 * �����������
 * �����������
 */
void Motor_Init(void)
{
    pwm_init(MotorL_pwm2,17000,0);
    pwm_init(MotorL_pwm1,17000,0);
    pwm_init(MotorR_pwm2,17000,0);
    pwm_init(MotorR_pwm1,17000,0);
}
/*
 * �������ܣ���·���PWM����
 * �������ƣ�MotorL_SetSpeed(uint8);
 * �����������Ҫ��PWMֵ
 * �����������
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
 * �������ܣ���·���PWM����
 * �������ƣ�MotorR_SetSpeed(uint8);
 * �����������Ҫ��PWMֵ
 * �����������
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
