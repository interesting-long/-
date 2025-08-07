/*
 * menu.c
 *
 *  Created on: 2025��7��13��
 *      Author: interesting
 */
#include "menu.h"
#include <stdio.h>

unsigned char Key;//���ڽ��ܼ���ֵ
char Key3_count=0;      //��¼����Key3�Ĵ���
char Start_Show_Row=0;  //�ӽṹ�����һ�п�ʼ��ʾ
float bujin=1;          //����ֵ
char Flag=1;            //������־λ
char mode=0;            //mode=1Ϊ����ģʽ
float Num[Num_Col][Page_Row_Number]={
        {-1,1,2,3,4,-1,-1,-1,-1},
        {-1,20,20,3,-1,-1,-1,-1,-1},
        {-1,1,2,50,50,0.8,-1,-1,-1},
        {-1,1,2,3,-1,-1,-1,-1,-1},
        {-1,1,2,3,-1,-1,-1,-1,-1},
        {-1,1,2,3,-1,-1,-1,-1,-1},
        {-1,1,2,3,-1,-1,-1,-1,-1}};//���ݴ洢������
char i,j;                 //����ѭ��
unsigned char Page0_children[Page_Row_Number+1]={0,1, 2,3,4};//���ֽڲ˵�,�ڶ�����ת��Page[1],Page[2]
unsigned char Page1_children[Page_Row_Number+1]={0, 5, 6};//���ֽڲ˵�,�ڶ�����ת��Page[5],Page[6]
unsigned char Show_image_mode=0;

/*�ṹ���һ����   ���кš����������ڿ�����ʾ���ݵ��߼�(���к��Ƕ��٣�����ʾ��һ���������)
 *�ṹ��ڶ�����   ��ʾ������
 *�ṹ���������   ��ָ����(�����Զ����ã�
 *�ṹ���������   ��ָ������(λ�ö�Ӧ����)�ú�������
 */
struct Menu Page[Struct_Number]={
    {   2,
            {"back","PID  ","PWM  ","Time","di_a_spe","Protect","duqu_line","         "}},
    {   0,
            {"back","Max_KP","Max_KD","Min_KP","Min_KI","ser_yu","      ","      ","       "}},
    {   1,
            {"back","R_PWM","L_PWM","M_Time","         ","         ","         ","         ","         "}},
    {   3,
            {"back","M_Time","sp_time","Slow_time","long_time","         ","         ","         ","         "}},
    {   4,
            {"back","diff_yu","diff_kp","speed_err","speed_add","         ","         ","         ","         "}},
    {   5,
            {"back","Page2-5","hallo","hallo","hallo","hallo","hallo","hallo","hallo"}},
    {   6,
            {"back","Page2-5","hallo","hallo","hallo","hallo","hallo","hallo","hallo"}},
};

struct Menu *Pin=&Page[0];//���ָ��
/*
 * ��������Ӳ˵��ĺ���
 * ����1:��Ҫ����Page[x]�ĵ�ַ;����2:��Ӧ������������(-1ֹͣ)
 * ����   Menu_SetChildren(&Page[0],Page0_children);  ����һҳ��2��3��4�е����ֽ�����ΪPage[1],Page[2],Page[3]
 * ��ע   ȷ������ĩβΪ-1Ϊ������־λ��0��������
*/
void Menu_SetChildren(struct Menu *parent, unsigned char child_indices[])
{
    // ���ԭ���ӽڵ�
    for(char i = 0; i < Page_Row_Number; i++) {
        parent->children[i] = NULL;
    }

    // �����µ��ӽڵ�
    for(int i = 0; child_indices[i] != -1 && i < Page_Row_Number; i++)
    {
        if(child_indices[i] > 0 && child_indices[i] < Struct_Number)
        {
            parent->children[i] = &Page[child_indices[i]];
            parent->children[i]->father = parent;
        }

    }
}


void menu_handle_key(unsigned char Flag_Key)
{
    Menu_SetChildren(&Page[0],Page0_children);//�趨���ֽ�
//    Menu_SetChildren(&Page[2],Page1_children);
    if(Flag_Key==1)//11111111111
        {
            if(mode==0)                                                         //MODE=0ʱ���߼�
            {
                if(Flag<Page_Row_Number)
                {
                    Flag++;                                                         //���¹����߼�(Key=1)
                }
                if(Flag<=max_OLED_ROW)
                {
                    oled_show_string(Show_arrow_x,(ROW_DISTANCE+1)*(Flag-2)," ");
                    oled_show_string(Show_arrow_x,(ROW_DISTANCE+1)*(Flag-1),"<");
                }
                else
                {
                    if(Flag>max_OLED_ROW ){Start_Show_Row++;}//��Flag����oled��ʾ��������������¹�����
                    if(Start_Show_Row>Page_Row_Number-max_OLED_ROW){Start_Show_Row=Page_Row_Number-max_OLED_ROW;}
                    if(Flag>Page_Row_Number){Flag=Page_Row_Number;}//��ֹԽ���������
                    oled_clear();
                    for(i=0;i<max_OLED_ROW;i++)                                  //**��ʾ�߼�**
                    {
                        oled_show_string(Show_String_x,(ROW_DISTANCE+1)*i,Pin->page[i+Start_Show_Row]);
                        if(Num[Pin->index][i+Start_Show_Row]!=0)
                        {
                            oled_show_float(Show_Num_x,(ROW_DISTANCE+1)*i,Num[Pin->index][i+Start_Show_Row],num_float_int,num_float_decimal);
                        }
                    }
                    oled_show_string(Show_arrow_x,(ROW_DISTANCE+1)*(max_OLED_ROW-1),"<");
                }
            }
            else if(mode==1)                       //����״̬�£�Key1�������Ӳ���ֵ   MODE=1ʱ���߼�
            {
                Num[Pin->index][Flag-1]+=bujin;
                for(i=0;i<max_OLED_ROW;i++)                                     //**��ʾ�߼�**
                {
                    oled_show_string(Show_String_x,(ROW_DISTANCE+1)*i,Pin->page[i+Start_Show_Row]);
                    if(Num[Pin->index][i+Start_Show_Row]!=0)
                    {
                        oled_show_float(Show_Num_x,(ROW_DISTANCE+1)*i,Num[Pin->index][i+Start_Show_Row],num_float_int,num_float_decimal);
                    }
                }
                if(Flag<=max_OLED_ROW)
                {
                    oled_show_string(Show_arrow_x,(ROW_DISTANCE+1)*(Flag-1),">");
                }
                else
                {
                    oled_show_string(Show_arrow_x,(ROW_DISTANCE+1)*(max_OLED_ROW-1),">");
                }
            }
        }
        if(Flag_Key==2)//22222222222
        {
            if(mode==0)                                                         //MODE=0ʱ���߼�
            {

            }
            else if(mode==1)                    //�޸Ĳ���ֵ{0.01->10}      �ı�MODE=1ʱ���߼�
            {
                if(bujin<=0.01)
                {
                    bujin=100;
                }
                bujin/=10.0;
                if(Flag<max_OLED_ROW)
                {
                    switch((int)(bujin*100))
                    {
                    case 1000:oled_show_string(Show_bujin_x,(ROW_DISTANCE+1)*(Flag-1),"10  ");break;
                    case 100:oled_show_string(Show_bujin_x,(ROW_DISTANCE+1)*(Flag-1),"1.0 ");break;
                    case 10:oled_show_string(Show_bujin_x,(ROW_DISTANCE+1)*(Flag-1),"0.1 ");break;
                    case 1:oled_show_string(Show_bujin_x,(ROW_DISTANCE+1)*(Flag-1),"0.01");break;
                    }
                }
                else
                {
                    switch((int)(bujin*100))
                    {
                    case 1000:oled_show_string(Show_bujin_x,(ROW_DISTANCE+1)*(max_OLED_ROW-1),"10  ");break;
                    case 100:oled_show_string(Show_bujin_x,(ROW_DISTANCE+1)*(max_OLED_ROW-1),"1.0 ");break;
                    case 10:oled_show_string(Show_bujin_x,(ROW_DISTANCE+1)*(max_OLED_ROW-1),"0.1 ");break;
                    case 1:oled_show_string(Show_bujin_x,(ROW_DISTANCE+1)*(max_OLED_ROW-1),"0.01");break;
                    }
                }
            }
        }
        if(Flag_Key==3)//333333333
        {
            Key3_count++;
        }
        if(Flag_Key==4)//4444444444
        {
            if(mode==1)                         //����״̬�£�Key4���ڼ�����ֵ      MODE=1ʱ���߼�
            {
                Num[Pin->index][Flag-1]-=bujin;

                for(i=0;i<max_OLED_ROW;i++)                                  //**��ʾ�߼�**
                {
                    oled_show_string(Show_String_x,(ROW_DISTANCE+1)*i,Pin->page[i+Start_Show_Row]);
                    if(Num[Pin->index][i+Start_Show_Row]!=0)
                    {
                        oled_show_float(Show_Num_x,(ROW_DISTANCE+1)*i,Num[Pin->index][i+Start_Show_Row],num_float_int,num_float_decimal);
                    }
                }
            }
            else if(mode==0)                                                 //MODE=0ʱ���߼�
            {
                if(Flag>1)
                {
                    Flag--;
                }
                if(Flag<max_OLED_ROW)                                          //���Ϲ����߼�
                {
                    if(Flag<0)
                    {Flag=0;}
                    oled_show_string(Show_arrow_x,(ROW_DISTANCE+1)*(Flag)," ");
                    oled_show_string(Show_arrow_x,(ROW_DISTANCE+1)*(Flag-1),"<");
                }
                else
                {
                    if(Start_Show_Row>0){Start_Show_Row--;}
                    oled_clear();
                    for(i=0;i<max_OLED_ROW;i++)                                //**��ʾ�߼�**
                    {
                        oled_show_string(Show_String_x,(ROW_DISTANCE+1)*i,Pin->page[i+Start_Show_Row]);
                    if(Num[Pin->index][i+Start_Show_Row]!=0)
                        {
                        oled_show_float(Show_Num_x,(ROW_DISTANCE+1)*i,Num[Pin->index][i+Start_Show_Row],num_float_int,num_float_decimal);
                        }
                    }
                    oled_show_string(Show_arrow_x,(ROW_DISTANCE+1)*(max_OLED_ROW-1),"<");
                }
            }
        }
        switch(Key3_count)//��������
        {
            case 1:                                                          //��1��Key3���߼�
            {
                if(Flag<max_OLED_ROW &&mode==0)                              //��ʾ�����>
                {
                    oled_show_string(Show_arrow_x,(ROW_DISTANCE+1)*(Flag-1),">");
                    mode=1;
                }
                else if(Flag>=max_OLED_ROW&&mode==0)
                {
                    oled_show_string(Show_arrow_x,(ROW_DISTANCE+1)*(max_OLED_ROW-1),">");
                    mode=1;
                }
                    break;
            }
            case 2:                                                             //��2��Key3���߼�
            {
                Flash_Save_Float(Num,0);
                if((Pin->children[Flag-1])!=NULL)//�������ֽڡ������������������
                {
                    Pin=Pin->children[Flag-1];
                    Flag=1;
                    Start_Show_Row=0;
                }
                else if(Pin->father!=NULL && Flag==1)//��һ�н��븸�ֽڡ������������˳�����
                {
                    Pin=Pin->father;
                    Flag=1;
                    Start_Show_Row=0;
                }
                Key3_count=0;mode=0;oled_clear();                 //���������û���Ӳ˵���ˢ����ʾ
                if (Flag <= max_OLED_ROW)
                {
                    oled_show_string(Show_arrow_x, (ROW_DISTANCE+1)*(Flag-1), "<");
                }
                else
                {
                        oled_show_string(Show_arrow_x, (ROW_DISTANCE+1)*(max_OLED_ROW-1), "<");
                    }
                for(i=0;i<max_OLED_ROW;i++)                                                     //��
                {                                                                               //ʾ
                    oled_show_string(Show_String_x,(ROW_DISTANCE+1)*i,Pin->page[i+Start_Show_Row]);              //��
                    if(Num[Pin->index][i+Start_Show_Row]!=0)                                     //��
                    {
                        oled_show_float(Show_Num_x,(ROW_DISTANCE+1)*i,Num[Pin->index][i+Start_Show_Row],num_float_int,num_float_decimal);
                    }
                }

                break;
            }
        }
    }

