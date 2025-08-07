/*
 * menu.h
 *
 *  Created on: 2025��7��13��
 *      Author: interesting
 */

#ifndef CODE_MENU_H_
#define CODE_MENU_H_
#include "zf_common_headfile.h"
#define Struct_Number 7         //�˵���ҳ��
#define Page_Row_Number 9       //һ���˵����������
#define Page_Col_Number 11      //һ�е����ַ�������󳤶�

#define max_OLED_ROW 8          //6*8��������Ļ���������ʾ������
#define ROW_DISTANCE     0          //�м��

#define page_Num 2              //�����ֵ�ҳ��
#define num_float_decimal  2       //���ݵ�С����λ��
#define num_float_int      3       //�����������ֵ�λ��
#define Num_Col 7              //�����ݵ�����   ���12����Flash����



#define Show_arrow_x  66        //��ʼ��ʾ��ͷ<��x����
#define Show_bujin_x  68        //��ʼ��ʾ����ֵ��x
#define Show_Num_x  90          //��ʼ��ʾ���ݵ�xֵ
#define Show_String_x  0       //��ʼ��ʾ�ַ�����x

extern unsigned char Key;
    struct Menu
    {
        unsigned char index;        //ҳ����
        char page[Page_Row_Number][Page_Col_Number];           //ҳ������
        struct Menu *father;        //���ڵ�
        struct Menu *children[Page_Row_Number];      //�ӽڵ�
    };

extern unsigned char Show_image_mode;
extern struct Menu *Pin;//���ָ��
extern float Num[Num_Col][Page_Row_Number];
void Menu_SetChildren(struct Menu *parent, unsigned char child_indices[]);
void menu_handle_key(unsigned char Flag_Key);




#define ser_Max_KP  Num[0][1]   //����KPֵ
#define ser_Max_KD  Num[0][2]   //����KDֵ
#define ser_Min_KP  Num[0][3]   //��С��KPֵ
#define ser_Min_KD  Num[0][4]   //��С��KDֵ
#define ser_yu      Num[0][5]   //���pidֱ��������ֵ

#define MorR_Max    Num[1][1]   //���ֵ�����ٶ�
#define MorL_Max    Num[1][2]   //���ֵ�����ٶ�

#define M_Time      Num[3][1]   //��ʱʱ�䣨s��
#define sp_time     Num[3][2]   //���ٵ��ж�ʱ��
#define jian_time   Num[3][3]   //���ٵ�ʱ��
#define long_time   Num[3][4]   //�жϳ�ֱ����ʱ��

#define diff_yu     Num[4][1]   //�жϲ��ٵ���ֵ
#define diff_kp     Num[4][2]   //���ٵ�kpֵ
#define speed_err   Num[4][3]   //ֱ�����ٵ���ֵ
#define speed_add   Num[4][4]   //ֱ�����ٴ�С


#define Protect     Num[2][5]
#define duqu_line   Num[2][6]


#endif /* CODE_MENU_H_ */
