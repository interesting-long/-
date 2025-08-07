/*
 * Key.h
 *
 *  Created on: 2025��7��14��
 *      Author: interesting
 */

#ifndef CODE_KEY_H_
#define CODE_KEY_H_

#define State_Release 0
#define State_Press_dou 1
#define State_Press 2
#define State_Release_dou 3
#define Time_dou 10
#define Lon_p_response_time    160//������Ӧʱ��
#define Lon_p_trigg             10//�����������

#include "zf_common_headfile.h"

extern unsigned char State;
extern unsigned char keynumber;
extern unsigned char Key_count;

unsigned char Key_Number(void);
void Key_scaner(void);
#endif /* CODE_KEY_H_ */

//IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
//{
//    interrupt_global_enable(0);                     // �����ж�Ƕ��
//    pit_clear_flag(CCU60_CH0);
//
//    keynumber=Key_Number();
//    switch(State)
//    {
//        case State_Release:     //�ɿ�״̬
//            if(keynumber!=0)
//            {
//                State=State_Press_dou;
//                Key_count=0;
//            }
//            break;
//
//        case State_Press_dou:   //���¶���״̬
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
//                    Key=keynumber;   // ��������
//                }
//            }
//            break;
//
//        case State_Press:      //����״̬
//            Key=0;              //!!!!!�޸�����ⰴ��һ˲���жϷ�������
//            if(keynumber==0)
//            {
//                State=State_Release_dou;
//                Key_count=0;
//                T0Count=0;
//            }
//            else
//            {
//                T0Count++;
//                if(T0Count>=500)  // ����500ms��
//                {
//                    Key=keynumber;  // ����ʱ�ظ�����
//                    // ������Ը�����Ҫ��ӳ��������⴦��
//                    T0Count=0;
//                }
//            }
//            break;
//
//        case State_Release_dou: //�ɿ�����״̬
//            if(keynumber==0)
//            {
//                Key_count++;
//                if(Key_count>=Time_dou)
//                {
//                    State=State_Release;
//                    Key=0;  // �ɿ�ʱ����
//                }
//            }
//            else
//            {
//                State=State_Press;
//            }
//            break;
//    }
//}
