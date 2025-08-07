/*
 * Key.h
 *
 *  Created on: 2025年7月14日
 *      Author: interesting
 */

#ifndef CODE_KEY_H_
#define CODE_KEY_H_

#define State_Release 0
#define State_Press_dou 1
#define State_Press 2
#define State_Release_dou 3
#define Time_dou 10
#define Lon_p_response_time    160//长按响应时间
#define Lon_p_trigg             10//长按触发间隔

#include "zf_common_headfile.h"

extern unsigned char State;
extern unsigned char keynumber;
extern unsigned char Key_count;

unsigned char Key_Number(void);
void Key_scaner(void);
#endif /* CODE_KEY_H_ */

//IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
//{
//    interrupt_global_enable(0);                     // 开启中断嵌套
//    pit_clear_flag(CCU60_CH0);
//
//    keynumber=Key_Number();
//    switch(State)
//    {
//        case State_Release:     //松开状态
//            if(keynumber!=0)
//            {
//                State=State_Press_dou;
//                Key_count=0;
//            }
//            break;
//
//        case State_Press_dou:   //按下抖动状态
//            if(keynumber==0)
//            {
//                State=State_Release;
//            }
//            else
//            {
//                Key_count++;
//                if(Key_count>=Time_dou)
//                {
//                    State=State_Press;
//                    Key=keynumber;   // 触发按键
//                }
//            }
//            break;
//
//        case State_Press:      //按下状态
//            Key=0;              //!!!!!修改项，避免按下一瞬间判断反复触发
//            if(keynumber==0)
//            {
//                State=State_Release_dou;
//                Key_count=0;
//                T0Count=0;
//            }
//            else
//            {
//                T0Count++;
//                if(T0Count>=500)  // 长按500ms后
//                {
//                    Key=keynumber;  // 长按时重复触发
//                    // 这里可以根据需要添加长按的特殊处理
//                    T0Count=0;
//                }
//            }
//            break;
//
//        case State_Release_dou: //松开抖动状态
//            if(keynumber==0)
//            {
//                Key_count++;
//                if(Key_count>=Time_dou)
//                {
//                    State=State_Release;
//                    Key=0;  // 松开时清零
//                }
//            }
//            else
//            {
//                State=State_Press;
//            }
//            break;
//    }
//}
