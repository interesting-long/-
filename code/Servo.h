/*
 * Servo.h
 *
 *  Created on: 2025Äê7ÔÂ15ÈÕ
 *      Author: interesting
 */

#ifndef CODE_SERVO_H_
#define CODE_SERVO_H_
#include "zf_common_headfile.h"


#define Servo_Pwm ATOM0_CH4_P21_6//ATOM0_CH1_P33_9

void Servo_Init(void);
void Ser_Servo_Duty(int value);

#endif /* CODE_SERVO_H_ */
