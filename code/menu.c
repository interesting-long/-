/*
 * menu.c
 *
 *  Created on: 2025年7月13日
 *      Author: interesting
 */
#include "menu.h"
#include <stdio.h>

unsigned char Key;//用于接受键码值
char Key3_count=0;      //记录按下Key3的次数
char Start_Show_Row=0;  //从结构体的哪一行开始显示
float bujin=1;          //步进值
char Flag=1;            //行数标志位
char mode=0;            //mode=1为锁定模式
float Num[Num_Col][Page_Row_Number]={
        {-1,1,2,3,4,-1,-1,-1,-1},
        {-1,20,20,3,-1,-1,-1,-1,-1},
        {-1,1,2,50,50,0.8,-1,-1,-1},
        {-1,1,2,3,-1,-1,-1,-1,-1},
        {-1,1,2,3,-1,-1,-1,-1,-1},
        {-1,1,2,3,-1,-1,-1,-1,-1},
        {-1,1,2,3,-1,-1,-1,-1,-1}};//数据存储的数组
char i,j;                 //用于循环
unsigned char Page0_children[Page_Row_Number+1]={0,1, 2,3,4};//子字节菜单,第二三行转到Page[1],Page[2]
unsigned char Page1_children[Page_Row_Number+1]={0, 5, 6};//子字节菜单,第二三行转到Page[5],Page[6]
unsigned char Show_image_mode=0;

/*结构体第一行是   序列号————用于控制显示数据的逻辑(序列号是多少，就显示哪一数组的数据)
 *结构体第二行是   显示的数据
 *结构体第三行是   父指针用(函数自动配置）
 *结构体第四行是   子指针数组(位置对应行数)用函数配置
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

struct Menu *Pin=&Page[0];//灵活指针
/*
 * 快捷设置子菜单的函数
 * 参数1:需要设置Page[x]的地址;参数2:对应的索引号数组(-1停止)
 * 例子   Menu_SetChildren(&Page[0],Page0_children);  将第一页的2，3，4行的子字节设置为Page[1],Page[2],Page[3]
 * 备注   确保数组末尾为-1为结束标志位，0跳过设置
*/
void Menu_SetChildren(struct Menu *parent, unsigned char child_indices[])
{
    // 清空原有子节点
    for(char i = 0; i < Page_Row_Number; i++) {
        parent->children[i] = NULL;
    }

    // 设置新的子节点
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
    Menu_SetChildren(&Page[0],Page0_children);//设定子字节
//    Menu_SetChildren(&Page[2],Page1_children);
    if(Flag_Key==1)//11111111111
        {
            if(mode==0)                                                         //MODE=0时的逻辑
            {
                if(Flag<Page_Row_Number)
                {
                    Flag++;                                                         //向下滚动逻辑(Key=1)
                }
                if(Flag<=max_OLED_ROW)
                {
                    oled_show_string(Show_arrow_x,(ROW_DISTANCE+1)*(Flag-2)," ");
                    oled_show_string(Show_arrow_x,(ROW_DISTANCE+1)*(Flag-1),"<");
                }
                else
                {
                    if(Flag>max_OLED_ROW ){Start_Show_Row++;}//当Flag大于oled显示的最大行数，向下滚动；
                    if(Start_Show_Row>Page_Row_Number-max_OLED_ROW){Start_Show_Row=Page_Row_Number-max_OLED_ROW;}
                    if(Flag>Page_Row_Number){Flag=Page_Row_Number;}//防止越界产生乱码
                    oled_clear();
                    for(i=0;i<max_OLED_ROW;i++)                                  //**显示逻辑**
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
            else if(mode==1)                       //锁定状态下，Key1用于增加步进值   MODE=1时的逻辑
            {
                Num[Pin->index][Flag-1]+=bujin;
                for(i=0;i<max_OLED_ROW;i++)                                     //**显示逻辑**
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
            if(mode==0)                                                         //MODE=0时的逻辑
            {

            }
            else if(mode==1)                    //修改步进值{0.01->10}      改变MODE=1时的逻辑
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
            if(mode==1)                         //锁定状态下，Key4用于减步进值      MODE=1时的逻辑
            {
                Num[Pin->index][Flag-1]-=bujin;

                for(i=0;i<max_OLED_ROW;i++)                                  //**显示逻辑**
                {
                    oled_show_string(Show_String_x,(ROW_DISTANCE+1)*i,Pin->page[i+Start_Show_Row]);
                    if(Num[Pin->index][i+Start_Show_Row]!=0)
                    {
                        oled_show_float(Show_Num_x,(ROW_DISTANCE+1)*i,Num[Pin->index][i+Start_Show_Row],num_float_int,num_float_decimal);
                    }
                }
            }
            else if(mode==0)                                                 //MODE=0时的逻辑
            {
                if(Flag>1)
                {
                    Flag--;
                }
                if(Flag<max_OLED_ROW)                                          //向上滚动逻辑
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
                    for(i=0;i<max_OLED_ROW;i++)                                //**显示逻辑**
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
        switch(Key3_count)//次数次数
        {
            case 1:                                                          //按1下Key3的逻辑
            {
                if(Flag<max_OLED_ROW &&mode==0)                              //显示反向箭>
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
            case 2:                                                             //按2下Key3的逻辑
            {
                Flash_Save_Float(Num,0);
                if((Pin->children[Flag-1])!=NULL)//进入子字节——————进入操作
                {
                    Pin=Pin->children[Flag-1];
                    Flag=1;
                    Start_Show_Row=0;
                }
                else if(Pin->father!=NULL && Flag==1)//第一行进入父字节——————退出操作
                {
                    Pin=Pin->father;
                    Flag=1;
                    Start_Show_Row=0;
                }
                Key3_count=0;mode=0;oled_clear();                 //按两下如果没有子菜单，刷新显示
                if (Flag <= max_OLED_ROW)
                {
                    oled_show_string(Show_arrow_x, (ROW_DISTANCE+1)*(Flag-1), "<");
                }
                else
                {
                        oled_show_string(Show_arrow_x, (ROW_DISTANCE+1)*(max_OLED_ROW-1), "<");
                    }
                for(i=0;i<max_OLED_ROW;i++)                                                     //显
                {                                                                               //示
                    oled_show_string(Show_String_x,(ROW_DISTANCE+1)*i,Pin->page[i+Start_Show_Row]);              //逻
                    if(Num[Pin->index][i+Start_Show_Row]!=0)                                     //辑
                    {
                        oled_show_float(Show_Num_x,(ROW_DISTANCE+1)*i,Num[Pin->index][i+Start_Show_Row],num_float_int,num_float_decimal);
                    }
                }

                break;
            }
        }
    }

