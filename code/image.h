/*
 * image.h
 *
 *  Created on: 2025年7月14日
 *      Author: interesting
 */

#ifndef CODE_IMAGE_H_
#define CODE_IMAGE_H_
#include "zf_common_headfile.h"

#define MID_W   MT9V03X_W/2
#define MID_H   MT9V03X_H/2
#define k_lim  0.8

extern unsigned char filtered[MT9V03X_H][MT9V03X_W];    //滤波之后的图像

unsigned char Ostu(uint16 index[MT9V03X_H][MT9V03X_W]);
unsigned char find_mid_line_final(void);

void Send_image_seekfree_assistant(void);
void Show_parameter(void);
void gaussian_filter(unsigned char input_index[MT9V03X_H][MT9V03X_W], unsigned char out_index[MT9V03X_H][MT9V03X_W], int width, int height) ;
#endif /* CODE_IMAGE_H_ */





