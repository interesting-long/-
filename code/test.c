/*
 * test.c
 *
 *  Created on: 2025Äê7ÔÂ28ÈÕ
 *      Author: interesting
 */

#include "test.h"

void Motor_test(void)
{
    Motor_Init();
    MotorL_SetSpeed(2000);
    MotorR_SetSpeed(2000);
}

void servo_test(void)
{
    Servo_Init();

    for(unsigned char i=770-80;i<770+80;i++)
    {
        Ser_Servo_Duty(i);
        system_delay_ms(20);
    }
    for(unsigned char i=770+80;i>770-80;i--)
    {
        Ser_Servo_Duty(i);
        system_delay_ms(20);
    }
}
