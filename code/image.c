/*
 * image.c
 *
 *  Created on: 2025��7��14��
 *      Author: interesting
 */

#include "image.h"

unsigned char filtered[MT9V03X_H][MT9V03X_W];


//-------------------------------------------------------------------------------------------------------------------
//  @brief      ���ٴ��
//  @return     uint8
//  @since      v1.1
//  Sample usage:   OTSU_Threshold = otsuThreshold(mt9v03x_image_dvp[0]);//�����ֵ
//-------------------------------------------------------------------------------------------------------------------
unsigned char Ostu(uint16 index[MT9V03X_H][MT9V03X_W])
{
#define GrayScale 256
float hist[GrayScale]={0};
float P[GrayScale]={0};
float PK[GrayScale]={0};
float MK[GrayScale]={0};
  uint8 threshold = 0;
  int imgsize = MT9V03X_H*MT9V03X_W;//�ܵ�ͼ����
  uint8 image_value_temp;   //�м����

   float sumPK = 0;//�����ۼƺ�
   float sumMK = 0; //�Ҷ��ۼӾ�ֵ
   float var = 0;
   float vartmp = 0 ;

   //����,��ֹ��һ��ͼƬ��Ӱ��
   for(uint16 i=0; i < GrayScale;i++)
   {
       hist[i]=0;
       P[i] = 0;
       PK[i] = 0;
       MK[i] = 0;
   }
    for(uint8 i=0 ; i<MT9V03X_H; i++)
    {
        for(uint8 j=0; j<MT9V03X_W; j++)
        {
            image_value_temp = index[i][j];
            hist[image_value_temp]++;
        }
    }
    for(uint16 i=0; i<GrayScale; i++)
    {
        P[i]= (float)hist[i]/imgsize; //����ÿ���Ҷȼ����ֵĸ���
        PK[i]= sumPK + P[i]; // �����ۼƺ�
        sumPK = PK[i];
        MK[i] = sumMK +i *P[i];//�Ҷ�ֵ�ۼӾ�ֵ
        sumMK = MK[i];
    }
    float total_mean = MK[GrayScale-1]; // ͼ���ܾ�ֵ
        for(uint8 i = 5; i < 245; i++) {
            float diff = total_mean * PK[i] - MK[i];
            float denom = PK[i] * (1 - PK[i] + 1e-5); // �������
             vartmp = (diff * diff) / denom;
            if(vartmp > var) {
                var = vartmp;
                threshold = i;
            }
        }
        return threshold ;
}


/**
 * @brief ��˹�˲�����ά����汾��
 * @param input_index  ����ͼ�񣨶�ά���飩
 * @param out_index    ���ͼ�񣨶�ά���飩
 * @param width        ͼ���ȣ�������
 * @param height       ͼ��߶ȣ�������
 */
void gaussian_filter(unsigned char input_index[MT9V03X_H][MT9V03X_W],
                     unsigned char out_index[MT9V03X_H][MT9V03X_W],
                     int width, int height)
{
    // 3x3 ��˹�ˣ�Ȩ���ܺ�=16��
    const float kernel[3][3] = {
        {1/16.0f, 2/16.0f, 1/16.0f},
        {2/16.0f, 4/16.0f, 2/16.0f},
        {1/16.0f, 2/16.0f, 1/16.0f}
    };

    // ����ͼ�񣨱ܿ��߽磩
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            float sum = 0;
            // 3x3 ������
            for (int m = -1; m <= 1; m++) {
                for (int n = -1; n <= 1; n++) {
                    sum += input_index[i + m][j + n] * kernel[m + 1][n + 1];
                }
            }
            out_index[i][j] = (unsigned char)(sum + k_lim); // ��������
        }
    }

    // �߽紦����ѡ�����0����
    for (int i = 0; i < height; i++) {
        out_index[i][0] = 0;                // ��߽�
        out_index[i][width - 1] = 0;         // �ұ߽�
    }
    for (int j = 0; j < width; j++) {
        out_index[0][j] = 0;                 // �ϱ߽�
        out_index[height - 1][j] = 0;        // �±߽�
    }
}
unsigned char Mid_wight_list[MT9V03X_H]=
{
        1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,
        6,6,6,6,6,6,6,6,6,6,
        6,7,8,9,10,11,10,9,8,7,
        6,6,6,6,6,6,6,6,6,6,
        1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1
};

unsigned char Last_mid_line=MID_W;
/*
 * ��������Ѱ�����յ�����ֵ
 * �����������
 * �����������Ȩ���ֱ��ֵ
 * ���ã��ҵ�Ŀ�����ߣ����ƶ��ת��
 * ���ӣ�Final_mid_line=find_mid_line_final();
 * ��ע��һ����Ѱ�����ߺ�ʹ��
 */
unsigned char find_mid_line_final(void)
{
    unsigned char mid_line_value=MID_W;
    unsigned char mid_line=MID_W;
    unsigned int mid_line_sum=0;
    unsigned int weight_sum=0;
    for(unsigned char i=Find_guai_point_start;i>Find_guai_point_end;i--)
    {
        mid_line_sum+=Mid_wight_list[i]*mid_line_list2[i];
        weight_sum+=Mid_wight_list[i];
    }
    mid_line=(unsigned char)(mid_line_sum/weight_sum);
    mid_line_value=Last_mid_line*0.2+mid_line*0.8;
    Last_mid_line=mid_line;
    return mid_line_value;
}

void Send_image_seekfree_assistant(void)
{
    // ����ͼ��
    seekfree_assistant_camera_send();
}

void Show_parameter(void)
{
    oled_show_string(0,0,"KP ");        //��ʾKP
    oled_show_string(0,1,"KI ");        //��ʾKI
    oled_show_string(0,2,"KD ");        //��ʾKD
    oled_show_string(0,3,"fang ");        //��ʾ����Ĵ�С
//    oled_show_string(0,4,"RM ");
//    oled_show_string(0,5,"LM ");
    oled_show_float(30,0,servo_pid.Kp,2,2);
    oled_show_float(30,1,servo_pid.Ki,2,2);
    oled_show_float(30,2,servo_pid.Kd,2,2);
    oled_show_int(30,3,Fang_Error,8);
//    oled_show_int(30,4,func_limit_ab(MorL_Max+(-err),3,80),2);
//    oled_show_int(30,5,func_limit_ab(MorR_Max+err,3,80),2);
}


