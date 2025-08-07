/*
 * Key.c
 *
 *  Created on: 2025��7��14��
 *      Author: interesting
 */
#include "Key.h"


int T0Count;
int T1Count;
unsigned char State;
unsigned char Key_count;
unsigned char keynumber;

unsigned char Key_Number(void)
{
    unsigned char KeyNumber=0;

    if(gpio_get_level(P11_9)==0){KeyNumber=1;}//P11_3//P11_9��**
    if(gpio_get_level(P11_6)==0){KeyNumber=2;}//P11_2//P11_6��**
    if(gpio_get_level(P11_12)==0){KeyNumber=3;}//P20_7//P11_12��**
    if(gpio_get_level(P11_10)==0){KeyNumber=4;}//P20_6//P11_10��**
    return KeyNumber;
}


void Key_scaner(void)
{
    keynumber=Key_Number();
        switch(State)
        {
            case State_Release:     //�ɿ�״̬
            {
                if(keynumber!=0)
                {
                    State=State_Press_dou;
                    Key_count=0;
                }
                break;
            }
            case State_Press_dou:   //���¶���״̬
            {
                if(keynumber==0)
                {
                    State=State_Release;
                }
                else
                {
                    Key_count++;
                    if(Key_count>=Time_dou)
                    {
                        State=State_Press;
                        Key=keynumber;   // ��������
                    }
                }
                break;
            }
            case State_Press:      //����״̬
            {
                if(keynumber==0)
                {
                    State=State_Release_dou;
                    Key_count=0;
                    T0Count=0;
                }
                else
                {
                    T0Count++;
                    if(T0Count>=Lon_p_response_time)  // ����500ms��
                    {
                        T1Count++;
                        if(T1Count>=Lon_p_trigg)
                        {
                            if(keynumber==3)//��������3�����л�����ģʽ���˶�ģʽ
                            {
                                if(Show_image_mode==0)
                                {
                                    Show_image_mode=1;
                                }
                                else
                                {
                                    oled_clear();
                                    Show_image_mode=0;
                                }
                                T0Count=0;
                            }
                            else if(keynumber==2)
                            {
                                oled_show_string(67,7,"Clear Over");
                                Clear_Num_ALL();//��������2�������NUm����
                                T0Count=0;
                            }
                            else
                            {
                                Key=keynumber;  // ����ʱ�ظ�����
                                // ������Ը�����Ҫ��ӳ��������⴦��
                                T1Count=0;
                            }
                        }
                    }
                }
                break;
            }
            case State_Release_dou: //�ɿ�����״̬
            {
                if(keynumber==0)
                {
                    Key_count++;
                    if(Key_count>=Time_dou)
                    {
                        State=State_Release;
                        Key=0;  // �ɿ�ʱ����
                    }
                }
                else
                {
                    State=State_Press;
                }
                break;
            }
        }
}
