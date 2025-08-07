/*
 * image2.h
 *
 *  Created on: 2025年7月27日
 *      Author: interesting
 */

#ifndef CODE_IMAGE2_H_
#define CODE_IMAGE2_H_

#include "zf_common_headfile.h"

#define start_column 5
#define end_column MT9V03X_W-5
#define IMG_WHITE 255
#define IMG_BLACK  0

#define Find_guai_point_start MT9V03X_H-1
#define Find_guai_point_end   5
#define limit_K 6
#define turn_k 0.05
#define find_error 10
#define left 2
#define right 1
#define vari 3
extern unsigned char base_image[MT9V03X_H][MT9V03X_W];  //用于接受灰度图像
extern unsigned char image[MT9V03X_H][MT9V03X_W];       //二值化后的图像
extern unsigned char left_line_list2[MT9V03X_H];        //左边线数组
extern unsigned char right_line_list2[MT9V03X_H];      //右边线数组
extern unsigned char mid_line_list2[MT9V03X_H];      //右边线数组
extern unsigned char vari_line_list[MT9V03X_H];                   //方差数组
extern unsigned char Left_Down_Point;
extern unsigned char Left_Up_Point;
extern unsigned char Right_Down_Point;
extern unsigned char Right_Up_Point;
extern unsigned char Both_Lost_Time;                 //两边丢线次数
extern unsigned char yuzhi;
extern unsigned char Final_mid_line;

unsigned char otsuThreshold(unsigned char *image);
void Set_image_twovalues(unsigned char value);
void cross_buxian(int x1, int x2, int y1, int y2,unsigned char num);
unsigned char Find_Left_Down_Point(int start, int end);
unsigned char Find_Left_Up_Point(int start, int end);
unsigned char Find_Right_Down_Point(int start, int end);
unsigned char Find_Right_Up_Point(int start, int end);
unsigned char X_Find_Left_Down_Point(int start, int end);
unsigned char X_Find_Left_Up_Point(int start, int end);
unsigned char X_Find_Right_Down_Point(int start, int end);
unsigned char X_Find_Right_Up_Point(int start, int end);
void Lengthen_Left_up(int start);
void Lengthen_Right_up(int start);
void Lengthen_Left_down(int start);
void Lengthen_Right_down(int start);
void Cross_Line(void);
void Right_Cross_line();
void Left_Cross_line();
void Longest_White_Colum(void);
void Lengthen_Left_down(int start);
void Lengthen_Right_down(int start);
void Cross_Line(void);
void Right_Cross_line();
void Left_Cross_line();
void Longest_White_Colum(void);
void Show_Third_Line2(void);
#endif /* CODE_INIT_H_ */
