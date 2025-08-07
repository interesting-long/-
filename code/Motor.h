/*
 * Motor.h
 *
 *  Created on: 2025��7��15��
 *      Author: interesting
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include "zf_common_headfile.h"

#define MotorL_pwm1 ATOM0_CH1_P21_3//ATOM0_CH4_P02_4
#define MotorL_pwm2 ATOM1_CH0_P21_2//ATOM0_CH5_P02_5
#define MotorR_pwm1 ATOM0_CH2_P21_4//ATOM0_CH6_P02_6
#define MotorR_pwm2 ATOM0_CH3_P21_5//ATOM0_CH7_P02_7

void Motor_Init(void);         //���IO�ڵĳ�ʼ��
void MotorL_SetSpeed(int pwm);//��������PWM//10000Ϊ�ٷ�֮һ��
void MotorR_SetSpeed(int pwm);//��������PWM
#endif /* CODE_MOTOR_H_ */
