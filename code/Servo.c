/*
 * Servo.c
 *
 *  Created on: 2025��7��15��
 *      Author: interesting
 */


#include "Servo.h"
void Servo_Init()
{
    system_delay_init();
    pwm_init(Servo_Pwm,50,duoji_mid);//�������766 ����� 840 �Ҵ��� 700
}

void Ser_Servo_Duty(int value)
{
    pwm_set_duty(Servo_Pwm,value);
}


//for(unsigned char i=0;i<40;i++)
//        {
//            duty+=5;
//            Ser_Servo_Duty(duty);
//
//        }
//        for(unsigned char i=0;i<40;i++)
//        {
//            duty-=5;
//            Ser_Servo_Duty(duty);
//        }
//        system_delay_ms(1000);
