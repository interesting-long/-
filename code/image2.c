/*
 * image2.c
 *
 *  Created on: 2025��7��27��
 *      Author: interesting
 */

#include "image2.h"
unsigned char base_image[MT9V03X_H][MT9V03X_W]; //ԭʼͼ��
unsigned char image[MT9V03X_H][MT9V03X_W];      //��ֵ��֮���ͼ������
unsigned char Final_mid_line=MID_W;

unsigned char left_line_list2[MT9V03X_H];        //���������
unsigned char right_line_list2[MT9V03X_H];      //�ұ�������
unsigned char mid_line_list2[MT9V03X_H];         //�м�������
unsigned char vari_line_list[MT9V03X_H];                   //��������
unsigned char White_Column[MT9V03X_W];          //�׵�ķֲ�ֱ��ͼ
unsigned char Left_Lost_Time = 0;                 //��߽綪�ߴ���
unsigned char Right_Lost_Time = 0;                //�ұ߽綪�ߴ���
unsigned char Both_Lost_Time = 0;                 //���߶��ߴ���
unsigned char Search_Stop_Line;                 //ֹͣ��ѯ��
unsigned char Longest_White_Column_Left[2];     //������е���Ϣ����
unsigned char Longest_White_Column_Right[2];    //������е���Ϣ����
unsigned char Longest_White_Column_Final[2];    //����ɨ�ߵ������
unsigned char Left_Lost_Flag[MT9V03X_H];        //��߽綪�߱�־����
unsigned char Right_Lost_Flag[MT9V03X_H];       //�ұ߽綪�߱�־����
unsigned char Boundry_Start_Left = 0;          //��һ���Ƕ��ߵ�,������߽���ʼ��
unsigned char Boundry_Start_Right = 0;          //��һ���Ƕ��ߵ�,�����ұ߽���ʼ��
//�յ�
unsigned char Left_Down_Point = 0;
unsigned char Left_Up_Point = 0;
unsigned char Right_Down_Point = 0;
unsigned char Right_Up_Point = 0;
unsigned char X_Left_Down_Point = 0;
unsigned char X_Left_Up_Point = 0;
unsigned char X_Right_Down_Point = 0;
unsigned char X_Right_Up_Point = 0;
unsigned char yuzhi;
static unsigned char left_search_flag = 0;
static unsigned char right_search_flag = 0;
static unsigned char right_start = 0;
static unsigned char left_start = 0;
unsigned char otsuThreshold(unsigned char *image)   //ע�������ֵ��һ��Ҫ��ԭͼ��
{
#define GrayScale 256
    int Pixel_Max=0;
    int Pixel_Min=255;
    int width = MT9V03X_W;
    int height = MT9V03X_H;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j, pixelSum = width * height;
    unsigned char threshold = 0;
    static unsigned char last_threshold;
    unsigned char* data = image;  //ָ���������ݵ�ָ��
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }

    uint32 gray_sum=0;
    //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
    for (i = 0; i < height; i+=1)
    {
        for (j = 0; j < width; j+=1)
        {
            pixelCount[(int)data[i * width + j]]++;  //����ǰ�ĵ������ֵ��Ϊ����������±�
            gray_sum+=(int)data[i * width + j];       //�Ҷ�ֵ�ܺ�
            if(data[i * width + j]>Pixel_Max)   Pixel_Max=data[i * width + j];
            if(data[i * width + j]<Pixel_Min)   Pixel_Min=data[i * width + j];
        }
    }

    //����ÿ������ֵ�ĵ�������ͼ���еı���

    for (i = Pixel_Min; i < Pixel_Max; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;

    }

    //�����Ҷȼ�[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;

    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = Pixel_Min; j < Pixel_Max; j++)
    {

        w0 += pixelPro[j];  //��������ÿ���Ҷ�ֵ�����ص���ռ����֮��   ���������ֵı���
        u0tmp += j * pixelPro[j];  //�������� ÿ���Ҷ�ֵ�ĵ�ı��� *�Ҷ�ֵ

        w1=1-w0;
        u1tmp=gray_sum/pixelSum-u0tmp;

        u0 = u0tmp / w0;              //����ƽ���Ҷ�
        u1 = u1tmp / w1;              //ǰ��ƽ���Ҷ�
        u = u0tmp + u1tmp;            //ȫ��ƽ���Ҷ�
        deltaTmp = (float)(w0 *w1* (u0 - u1)* (u0 - u1)) ;
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
        }

    }

    if(threshold>75 && threshold<200)
        last_threshold = threshold;
    else if(last_threshold!=0)
        threshold = last_threshold;

    return threshold;
}
/*��������base_image���մ������ֵ����ֵ��֮�����image����
 *
 *
 */
void Set_image_twovalues(unsigned char value)
{
    unsigned char tempt_value,j,i;
    for(i=0;i<MT9V03X_H;i++)
    {
        for(j=0;j<MT9V03X_W;j++)
        {
            tempt_value=base_image[i][j];
            if(tempt_value<value)
                image[i][j]=0;
            else
                image[i][j]=255;
        }
    }
}
/*
 * �����������ߺ���
 * �������1����ʼ���x����
 * �������2���������x����
 * �������3����ʼ���y����
 * �������4���������y����
* �������5����������
 *
 */
void cross_buxian(int x1, int x2, int y1, int y2,unsigned char num) {
    static float left_last_K = 0;
    static float right_last_K = 0;
    // �߽��飨���ֲ��䣩
    if (x1 >= MT9V03X_W - 1) x1 = MT9V03X_W - 1;
    else if (x1 < 0) x1 = 0; // ע�⣺ԭ������ <=0�������Ϊ <0 ������

    if (y1 >= MT9V03X_H - 1) y1 = MT9V03X_H - 1;
    else if (y1 < 0) y1 = 0;

    if (x2 >= MT9V03X_W - 1) x2 = MT9V03X_W - 1;
    else if (x2 < 0) x2 = 0;

    if (y2 >= MT9V03X_H - 1) y2 = MT9V03X_H - 1;
    else if (y2 < 0) y2 = 0;

    int a1 = y1+1, a2 = y2-1;
    int b1 = x1+1, b2 = x2-1;

    // ȷ�� a1 <= a2�����������꣩
    if (a1 > a2) {
        int temp = a1; a1 = a2; a2 = temp;
        temp = b1; b1 = b2; b2 = temp;
    }

    // ����ֱ�߻򵥵㣨y1 == y2��
    if (a1 == a2) {
        // ˮƽ�ߣ�ֻ������һ���㣨�������߶κ�������ͬ��
        if (a1 >= 0 && a1 < MT9V03X_H) {
            // ȡ b1 �� b2 ���ɣ�����ѡ b1
            if (num == 1)
            {
                right_line_list2[a1] = (b1 < 0) ? 0 : ((b1 >= MT9V03X_W) ? MT9V03X_W - 1 : b1);
            }
            else if (num == 2)
            {
                left_line_list2[a1] = (b1 < 0) ? 0 : ((b1 >= MT9V03X_W) ? MT9V03X_W - 1 : b1);
            }

        }
        return;
    }

    // ����б�ʣ�ʹ�ø������ضϣ�
    float K = (float)(b2 - b1) / (float)(a2 - a1); // �ؼ��޸ģ��������
    if (num == 2)
    {
        if (K < limit_K)
        {
            left_last_K = K;
        }
        else if (left_last_K != 0 && fabs(left_last_K - K) > turn_k)
        {
            K = left_last_K;
        }
    }
    else if (num == 1)
    {
        if (K < limit_K)
        {
            right_last_K = K;
        }
        else if (right_last_K != 0 && fabs(right_last_K - K) > turn_k)
        {
            K = right_last_K;
        }
    }

    for (int i = a1; i <= a2; i++)
    {
        // ��������꣨����������߾��ȣ�
        int x = b1 + (int)((i - a1) * K + 0.5);

        // Լ�������굽ͼ��߽�
        if (x < 0) x = 0;
        else if (x >= MT9V03X_W) x = MT9V03X_W - 1;

        // �洢���ұ�������
        if (num == 1)
        {
            if (i >= 0 && i < MT9V03X_H)
            {
                right_line_list2[i] = (unsigned char)x;
            }
        }
        else if (num == 2)
        {
            if (i >= 0 && i < MT9V03X_H)
            {
                left_line_list2[i] = (unsigned char)x;
            }
        }
        else if (num == 3)
        {
            if (i >= 0 && i < MT9V03X_H)
            {
                vari_line_list[i] = (unsigned char)x;
            }
        }

    }
}
unsigned char Find_Left_Down_Point(int start, int end)
{
    int i, t;
    unsigned char left_down_line = 0;
    if (Left_Lost_Time >= 0.95 * MT9V03X_H)
    {
        return left_down_line;
    }
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5)//����5������5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start = MT9V03X_H - 1 - 5;//��һ���棬���жϵ�i��ʱ������ʵ�i+3��i-4�У���ֹԽ��
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (left_down_line == 0 &&
            abs(left_line_list2[i] - left_line_list2[i + 1]) <= 5 &&
            abs(left_line_list2[i + 1] - left_line_list2[i + 2]) <= 5 &&
            abs(left_line_list2[i + 2] - left_line_list2[i + 3]) <= 5 &&
            (left_line_list2[i] - left_line_list2[i - 1]) >= 3 &&
            (left_line_list2[i] - left_line_list2[i - 2]) >= 5 &&
            (left_line_list2[i] - left_line_list2[i - 3]) >= 7)
        {
            left_down_line = i;
            break;
        }
    }
    return left_down_line;
}
unsigned char Find_Left_Up_Point(int start, int end)
{
    int i, t;
    unsigned char left_Up_line = 0;
    if (Left_Lost_Time >= 0.95 * MT9V03X_H)
    {
        return left_Up_line;
    }
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5)//����5������5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start = MT9V03X_H - 1 - 5;//��һ���棬���жϵ�i��ʱ������ʵ�i+3��i-4�У���ֹԽ��
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (left_Up_line == 0 &&
            abs(left_line_list2[i] - left_line_list2[i - 1]) <= 5 &&
            abs(left_line_list2[i - 1] - left_line_list2[i - 2]) <= 5 &&
            abs(left_line_list2[i - 2] - left_line_list2[i - 3]) <= 5 &&
            (left_line_list2[i] - left_line_list2[i + 1]) >= 3 &&
            (left_line_list2[i] - left_line_list2[i + 2]) >= 5 &&
            (left_line_list2[i] - left_line_list2[i + 3]) >= 7)
        {
            left_Up_line = i;
            break;
        }
    }
    return left_Up_line;
}
unsigned char Find_Right_Down_Point(int start, int end)
{
    int i, t;
    unsigned char right_down_line = 0;
    if (Right_Lost_Time >= 0.95 * MT9V03X_H)
    {
        return right_down_line;
    }
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5)//����5������5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start = MT9V03X_H - 1 - 5;//��һ���棬���жϵ�i��ʱ������ʵ�i+3��i-4�У���ֹԽ��
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (right_down_line == 0 &&
            abs(right_line_list2[i] - right_line_list2[i + 1]) <= 3 &&
            abs(right_line_list2[i + 1] - right_line_list2[i + 2]) <= 3 &&
            abs(right_line_list2[i + 2] - right_line_list2[i + 3]) <= 3 &&
            (right_line_list2[i - 1] - right_line_list2[i]) >= 3&&
            (right_line_list2[i - 2] - right_line_list2[i]) >= 5 &&
            (right_line_list2[i - 3] - right_line_list2[i]) >= 7)
        {
            right_down_line = i;
            break;
        }
    }
    return right_down_line;
}
unsigned char Find_Right_Up_Point(int start, int end)
{
    int i, t;
    unsigned char right_up_line = 0;
    if (Right_Lost_Time >= 0.95 * MT9V03X_H)
    {
        return right_up_line;
    }
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5)//����5������5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start = MT9V03X_H - 1 - 5;//��һ���棬���жϵ�i��ʱ������ʵ�i+3��i-4�У���ֹԽ��
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (right_up_line == 0 &&
            abs(right_line_list2[i] - right_line_list2[i - 1]) <= 3  &&
            abs(right_line_list2[i - 1] - right_line_list2[i - 2]) <= 3 &&
            abs(right_line_list2[i - 2] - right_line_list2[i - 3]) <= 3 &&
            (right_line_list2[i + 1] - right_line_list2[i]) >= 3 &&
            (right_line_list2[i + 2] - right_line_list2[i]) >= 5 &&
            (right_line_list2[i + 3] - right_line_list2[i]) >= 7)
        {
            right_up_line = i;
            break;
        }
    }
    return right_up_line;
}
/************************б��յ��ж�*******�·�*************************************/
unsigned char X_Find_Left_Down_Point(int start, int end)
{
    int i, t;
    unsigned char left_down_line = 0;
    if (Left_Lost_Time >= 0.95 * MT9V03X_H)
    {
        return left_down_line;
    }
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5)//����5������5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start = MT9V03X_H - 1 - 5;//��һ���棬���жϵ�i��ʱ������ʵ�i+3��i-4�У���ֹԽ��
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (left_down_line == 0 &&
            (left_line_list2[i] - left_line_list2[i + 1]) <= 3 &&
            (left_line_list2[i + 1] - left_line_list2[i + 2]) <= 3 &&
            (left_line_list2[i + 2] - left_line_list2[i + 3]) <= 3 &&
            (left_line_list2[i] - left_line_list2[i + 1]) >=-1 &&
            (left_line_list2[i + 1] - left_line_list2[i + 2]) >= -1 &&
            (left_line_list2[i + 2] - left_line_list2[i + 3]) >= -1 &&
            (left_line_list2[i] - left_line_list2[i - 1]) >= 2 &&
            (left_line_list2[i] - left_line_list2[i - 2]) >= 3 &&
            (left_line_list2[i] - left_line_list2[i - 3]) >= 4)
        {
            left_down_line = i;
            break;
        }
    }
    return left_down_line;
}
unsigned char X_Find_Left_Up_Point(int start, int end)
{
    int i, t;
    unsigned char left_Up_line = 0;
    if (Left_Lost_Time >= 0.95 * MT9V03X_H)
    {
        return left_Up_line;
    }
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5)//����5������5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start = MT9V03X_H - 1 - 5;//��һ���棬���жϵ�i��ʱ������ʵ�i+3��i-4�У���ֹԽ��
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (left_Up_line == 0 &&
            (left_line_list2[i - 1] - left_line_list2[i]) <= 3 &&
            (left_line_list2[i - 2] - left_line_list2[i - 1]) <= 3 &&
            (left_line_list2[i - 3] - left_line_list2[i - 2]) <= 3 &&
            (left_line_list2[i - 1] - left_line_list2[i]) >= 0 &&
            (left_line_list2[i - 2] - left_line_list2[i - 1]) >= 0 &&
            (left_line_list2[i - 3] - left_line_list2[i - 2]) >= 0 &&
            (left_line_list2[i] - left_line_list2[i + 1]) >= 7 &&
            (left_line_list2[i] - left_line_list2[i + 2]) >= 10 &&
            (left_line_list2[i] - left_line_list2[i + 3]) >= 15)
        {
            left_Up_line = i;
            break;
        }
    }
    return left_Up_line;
}
unsigned char X_Find_Right_Down_Point(int start, int end)
{
    int i, t;
    unsigned char right_down_line = 0;
    if (Right_Lost_Time >= 0.95 * MT9V03X_H)
    {
        return right_down_line;
    }
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5)//����5������5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start = MT9V03X_H - 1 - 5;//��һ���棬���жϵ�i��ʱ������ʵ�i+3��i-4�У���ֹԽ��
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (right_down_line == 0 &&
            (right_line_list2[i] - right_line_list2[i + 1]) <= 3 &&
            (right_line_list2[i + 1] - right_line_list2[i + 2]) <= 3 &&
            (right_line_list2[i + 2] - right_line_list2[i + 3]) <= 3 &&
            (right_line_list2[i] - right_line_list2[i + 1]) >= -1 &&
            (right_line_list2[i + 1] - right_line_list2[i + 2]) >= -1 &&
            (right_line_list2[i + 2] - right_line_list2[i + 3]) >= -1&&
            (right_line_list2[i - 1] - right_line_list2[i]) >= 2 &&
            (right_line_list2[i - 2] - right_line_list2[i]) >= 3 &&
            (right_line_list2[i - 3] - right_line_list2[i]) >= 4)
        {
            right_down_line = i;
            break;
        }
    }
    return right_down_line;
}
unsigned char X_Find_Right_Up_Point(int start, int end)
{
    int i, t;
    unsigned char right_up_line = 0;
    if (Right_Lost_Time >= 0.95 * MT9V03X_H)
    {
        return right_up_line;
    }
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5)//����5������5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start = MT9V03X_H - 1 - 5;//��һ���棬���жϵ�i��ʱ������ʵ�i+3��i-4�У���ֹԽ��
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (right_up_line == 0 &&
            (right_line_list2[i] - right_line_list2[i - 1]) <= 3 &&
            (right_line_list2[i - 1] - right_line_list2[i - 2]) <= 3 &&
            (right_line_list2[i - 2] - right_line_list2[i - 3]) <= 3 &&
            (right_line_list2[i] - right_line_list2[i - 1]) >= -1 &&
            (right_line_list2[i - 1] - right_line_list2[i - 2]) >= -1&&
            (right_line_list2[i - 2] - right_line_list2[i - 3]) >= -1 &&
            (right_line_list2[i + 2] - right_line_list2[i]) >= 7 &&
            (right_line_list2[i + 3] - right_line_list2[i]) >= 10 &&
            (right_line_list2[i + 4] - right_line_list2[i]) >= 15)
        {
            right_up_line = i;
            break;
        }
    }
    return right_up_line;
}
/************************б��յ��ж�*******�Ϸ�*************************************/
float Slope_Calculate(unsigned char begin, unsigned char end, unsigned char* border)
{
    float xsum = 0, ysum = 0, xysum = 0, x2sum = 0;
    int i = 0;
    float result = 0;
    static float resultlast;

    for (i = begin; i < end; i++)
    {
        xsum += i;
        ysum += border[i];
        xysum += i * (border[i]);
        x2sum += i * i;

    }
    if ((end - begin) * x2sum - xsum * xsum) //�жϳ����Ƿ�Ϊ��
    {
        result = ((end - begin) * xysum - xsum * ysum) / ((end - begin) * x2sum - xsum * xsum);
        resultlast = result;
    }
    else
    {
        result = resultlast;
    }
    return result;
}
void Lengthen_Left_up(int start)
{
    int i;
    float k;
    static float last_K = 0;
    if (start > MT9V03X_H - 1)
    {
        start = MT9V03X_H - 1;
    }
    else if (start <= 0)
    {
        start = 0;
    }
    if (start < MT9V03X_H - 8)
    {
        //        k=(float)(left_line_list2[start+2]-left_line_list2[start+4])/2.0;
        k = Slope_Calculate(start, start + 7, left_line_list2);
        if (k < limit_K)
        {
            last_K = k;
        }
        else if(last_K!=0 && abs(last_K - k) > turn_k)
        {
            k = last_K;
        }
        for (i = start+3; i > 1; i--)
        {
            left_line_list2[i] = left_line_list2[start+3] - (int)((start+3 - i) * k + 0.5);
            if (left_line_list2[i] >= MT9V03X_W - 1)
            {
                left_line_list2[i] = 0;
            }
            else if (left_line_list2[i] <= 0)
            {
                left_line_list2[i] = 0;
            }
        }
    }
}

void Lengthen_Right_up(int start)
{
    int i;
    float k;
    static float last_K = 0;
    if (start > MT9V03X_H - 1)
    {
        start = MT9V03X_H - 1;
    }
    else if (start <= 0)
    {
        start = 0;
    }
    if (start < MT9V03X_H - 8)
    {
        //        k=(float)(right_line_list2[start+2]-right_line_list2[start+4])/2.0;
        k = Slope_Calculate(start, start + 7, right_line_list2);
        if (k < limit_K)
        {
            last_K = k;
        }
        else if (last_K != 0 && abs(last_K - k) > turn_k)
        {
            k = last_K;
        }
        for (i = start+3; i > 1; i--)
        {
            right_line_list2[i] = right_line_list2[start+3] - (int)((start+3 - i) * k + 0.5);
            if (right_line_list2[i] >= MT9V03X_W - 1)
            {
                right_line_list2[i] = MT9V03X_W - 1;
            }
            else if (right_line_list2[i] <= 0)
            {
                right_line_list2[i] = 0;
            }
        }
    }
}
void Lengthen_Left_down(int start)
{
    int i;
    float k;
    static float last_K=0;
    if (start > MT9V03X_H - 1)
    {
        start = MT9V03X_H - 1;
    }
    else if (start <= 0)
    {
        start = 0;
    }
    if (start > 8)
    {
        //        k=(float)(left_line_list2[start-2]-left_line_list2[start-4])/2.0;
        k = Slope_Calculate(start - 7, start, left_line_list2);
        if (k < limit_K)
        {
            last_K = k;
        }
        else if (last_K != 0 && abs(last_K - k) > turn_k)
        {
            k = last_K;
        }
        for (i = start-3; i < MT9V03X_H - 1; i++)
        {
            left_line_list2[i] = left_line_list2[start-3] + (int)((i - start+3) * k + 0.5);
            if (left_line_list2[i] >= MT9V03X_W - 1)
            {
                left_line_list2[i] = 0;
            }
            else if (left_line_list2[i] <= 0 )
            {
                left_line_list2[i] = 0;
            }
        }
    }
}
void Lengthen_Right_down(int start)
{
    int i;
    float k;
    static float last_K = 0;
    if (start > MT9V03X_H - 1)
    {
        start = MT9V03X_H - 1;
    }
    else if (start <= 0)
    {
        start = 0;
    }
    if (start > 8)
    {
        //        k=(float)(right_line_list2[start-2]-right_line_list2[start-4])/2.0;
        k = Slope_Calculate(start - 7, start, right_line_list2);
        if (k < limit_K)
        {
            last_K = k;
        }
        else if (last_K != 0 && abs(last_K-k)> turn_k)
        {
            k = last_K;
        }
        for (i = start-3; i < MT9V03X_H - 1; i++)
        {
            right_line_list2[i] = right_line_list2[start-3] + (int)((i - start+3) * k + 0.5);
            if (right_line_list2[i] >= MT9V03X_W - 1)
            {
                right_line_list2[i] = MT9V03X_W - 1;
            }
            else if (right_line_list2[i] <= 0)
            {
                right_line_list2[i] = 0;
            }
        }
    }
}
void Cross_Line(void)
{

    Left_Down_Point = Find_Left_Down_Point(Find_guai_point_start, Find_guai_point_end);
    Left_Up_Point = Find_Left_Up_Point(Find_guai_point_start, Find_guai_point_end);
    Right_Down_Point = Find_Right_Down_Point(Find_guai_point_start, Find_guai_point_end);
    Right_Up_Point = Find_Right_Up_Point(Find_guai_point_start, Find_guai_point_end);
    if ((Right_Up_Point != 0 && Left_Up_Point==0) || (Right_Down_Point!=0 && Left_Down_Point==0))
    {
        left_search_flag = 1;
        right_search_flag = 0;
        left_start = right_line_list2[Right_Up_Point];
    }
    else if ((Right_Up_Point == 0 && Left_Up_Point != 0) || (Right_Down_Point == 0 && Left_Down_Point != 0))
    {
        left_search_flag = 0;
        right_search_flag = 1;
        right_start = left_line_list2[Left_Up_Point];
    }

    if (Left_Down_Point < Left_Up_Point)
    {
        Left_Down_Point = 0;
    }
    if (Right_Down_Point < Right_Up_Point)
    {
        Right_Down_Point = 0;
    }
    if (Left_Down_Point != 0 && Left_Up_Point != 0 && Right_Down_Point != 0 && Right_Up_Point != 0)//�ĸ��յ㣺����
    {
        cross_buxian(left_line_list2[Left_Down_Point], left_line_list2[Left_Up_Point], Left_Down_Point, Left_Up_Point, left);
        cross_buxian(right_line_list2[Right_Down_Point], right_line_list2[Right_Up_Point], Right_Down_Point, Right_Up_Point,right);
    }
    else if (Left_Down_Point != 0 && Left_Up_Point == 0 && Right_Down_Point != 0 && Right_Up_Point == 0)//ֻ�ҵ��Ϲյ㣻�ӳ��ϱ���
    {
        Lengthen_Left_up(Left_Down_Point);
        Lengthen_Right_up(Right_Down_Point);
    }
    else if (Left_Down_Point == 0 && Left_Up_Point != 0 && Right_Down_Point == 0 && Right_Up_Point != 0)//ֻ�ҵ��¹յ㣻�ӳ��±���
    {
        Lengthen_Left_down(Left_Up_Point);
        Lengthen_Right_down(Right_Up_Point);
    }
    else if (Left_Down_Point != 0 && Left_Up_Point != 0 && Right_Down_Point == 0 && Right_Up_Point != 0)
    {
        cross_buxian(left_line_list2[Left_Down_Point], left_line_list2[Left_Up_Point], Left_Down_Point, Left_Up_Point, left);
        Lengthen_Right_down(Right_Up_Point);
    }
    else if (Left_Down_Point == 0 && Left_Up_Point != 0 && Right_Down_Point != 0 && Right_Up_Point != 0)
    {
        cross_buxian(right_line_list2[Right_Down_Point], right_line_list2[Right_Up_Point], Right_Down_Point, Right_Up_Point, right);
        Lengthen_Left_down(Left_Up_Point);
    }
    else if (Left_Down_Point != 0 && Left_Up_Point != 0 && Right_Down_Point != 0 && Right_Up_Point == 0)
    {
        cross_buxian(left_line_list2[Left_Down_Point], left_line_list2[Left_Up_Point], Left_Down_Point, Left_Up_Point, left);
        Lengthen_Right_up(Right_Down_Point);
    }
    else if (Left_Down_Point != 0 && Left_Up_Point == 0 && Right_Down_Point != 0 && Right_Up_Point != 0)
    {
        cross_buxian(right_line_list2[Right_Down_Point], right_line_list2[Right_Up_Point], Right_Down_Point, Right_Up_Point, right);
        Lengthen_Left_up(Left_Down_Point);
    }
    else if (Left_Down_Point == 0 && Left_Up_Point != 0 && Right_Down_Point == 0 && Right_Up_Point == 0)
    {
        Lengthen_Left_down(Left_Up_Point);
    }
    else if (Left_Down_Point == 0 && Left_Up_Point == 0 && Right_Down_Point == 0 && Right_Up_Point != 0)
    {
        Lengthen_Right_down(Right_Up_Point);
    }
    //else if (Left_Down_Point != 0 && Left_Up_Point == 0 && Right_Down_Point == 0 && Right_Up_Point == 0)
    //{
    //    Lengthen_Left_up(Left_Down_Point);
    //}
    //else if (Left_Down_Point == 0 && Left_Up_Point == 0 && Right_Down_Point != 0 && Right_Up_Point == 0)
    //{
    //    Lengthen_Right_up(Right_Down_Point);
    //}

}

void Right_Cross_line()
{
    X_Left_Down_Point = X_Find_Left_Down_Point(Find_guai_point_start, Find_guai_point_end);
    X_Left_Up_Point = X_Find_Left_Up_Point(Find_guai_point_start, Find_guai_point_end);
    X_Right_Down_Point = X_Find_Right_Down_Point(Find_guai_point_start, Find_guai_point_end);
    X_Right_Up_Point = X_Find_Right_Up_Point(Find_guai_point_start, Find_guai_point_end);
    //if (X_Left_Down_Point < X_Left_Up_Point)
    //{
    //    X_Left_Down_Point = 0;
    //}
    //if (X_Right_Down_Point < X_Right_Up_Point)
    //{
    //    X_Right_Down_Point = 0;
    //}
    if ((X_Right_Up_Point != 0 && X_Left_Up_Point == 0) || (X_Right_Down_Point != 0 && X_Left_Down_Point == 0))
    {
        left_search_flag = 1;
        right_search_flag = 0;
        left_start = right_line_list2[X_Right_Up_Point];
    }
    else if((X_Right_Up_Point == 0 && X_Left_Up_Point != 0) || (X_Right_Down_Point == 0 && X_Left_Down_Point != 0))
    {
        left_search_flag = 0;
        right_search_flag = 1;
        right_start = left_line_list2[X_Left_Up_Point];
    }
    if (X_Left_Down_Point != 0 && X_Left_Up_Point != 0 && X_Right_Down_Point != 0 && X_Right_Up_Point != 0)//�ĸ��յ㣺����
    {
        cross_buxian(left_line_list2[X_Left_Down_Point], left_line_list2[X_Left_Up_Point], X_Left_Down_Point, X_Left_Up_Point, left);
        cross_buxian(right_line_list2[X_Right_Down_Point], right_line_list2[X_Right_Up_Point], X_Right_Down_Point, X_Right_Up_Point, right);
    }
    else if (X_Left_Down_Point == 0 && X_Left_Up_Point != 0 && X_Right_Down_Point != 0 && X_Right_Up_Point != 0)//����
    {
        cross_buxian(right_line_list2[X_Right_Down_Point], right_line_list2[X_Right_Up_Point], X_Right_Down_Point, X_Right_Up_Point, right);
        Lengthen_Left_down(X_Left_Up_Point);
    }
    else if (X_Left_Down_Point != 0 && X_Left_Up_Point != 0 && X_Right_Down_Point == 0 && X_Right_Up_Point != 0)//��б����
    {
        cross_buxian(left_line_list2[X_Left_Down_Point], left_line_list2[X_Left_Up_Point], X_Left_Down_Point, X_Left_Up_Point, right);
        Lengthen_Right_down(X_Right_Up_Point);
    }
    else if (X_Right_Down_Point != 0 && X_Right_Up_Point != 0 && X_Left_Down_Point == 0 && X_Left_Up_Point == 0)
    {
        cross_buxian(right_line_list2[X_Right_Down_Point], right_line_list2[X_Right_Up_Point], X_Right_Down_Point, X_Right_Up_Point, right);
    }
    //else if (Left_Down_Point == 0 && Left_Up_Point != 0 && Right_Down_Point == 0 && Right_Up_Point == 0)
    //{
    //    Lengthen_Left_down(Left_Up_Point);
    //}
    //else if (Left_Down_Point == 0 && Left_Up_Point == 0 && Right_Down_Point == 0 && Right_Up_Point != 0)
    //{
    //    Lengthen_Right_down(Right_Up_Point);
    //}
    else if (X_Left_Down_Point != 0 && X_Left_Up_Point == 0 && X_Right_Down_Point == 0 && X_Right_Up_Point == 0)//����
    {
        Lengthen_Left_up(X_Left_Down_Point);
    }
    else if (X_Left_Down_Point == 0 && X_Left_Up_Point == 0 && X_Right_Down_Point != 0 && X_Right_Up_Point == 0)//����
    {
        Lengthen_Right_up(X_Right_Down_Point);
    }
}

void Left_Cross_line()
{
    X_Left_Down_Point = X_Find_Left_Down_Point(Find_guai_point_start, Find_guai_point_end);
    X_Left_Up_Point = X_Find_Left_Up_Point(Find_guai_point_start, Find_guai_point_end);
    X_Right_Down_Point = X_Find_Right_Down_Point(Find_guai_point_start, Find_guai_point_end);
    X_Right_Up_Point = X_Find_Right_Up_Point(Find_guai_point_start, Find_guai_point_end);
    if (X_Left_Down_Point < X_Left_Up_Point)
    {
        X_Left_Down_Point = 0;
    }
    if (X_Right_Down_Point < X_Right_Up_Point)
    {
        X_Right_Down_Point = 0;
    }
    if ((X_Right_Up_Point != 0 && X_Left_Up_Point == 0) || (X_Right_Down_Point != 0 && X_Left_Down_Point == 0))
    {
        left_search_flag = 1;
        right_search_flag = 0;
        left_start = right_line_list2[X_Right_Up_Point];
    }
    else if ((X_Right_Up_Point == 0 && X_Left_Up_Point != 0) || (X_Right_Down_Point == 0 && X_Left_Down_Point != 0))
    {
        left_search_flag = 0;
        right_search_flag = 1;
        right_start = left_line_list2[X_Left_Up_Point];
    }
    if (X_Left_Down_Point != 0 && X_Left_Up_Point != 0 && X_Right_Down_Point != 0 && X_Right_Up_Point != 0)//�ĸ��յ㣺����
    {
        cross_buxian(left_line_list2[X_Left_Down_Point], left_line_list2[X_Left_Up_Point], X_Left_Down_Point, X_Left_Up_Point, left);
        cross_buxian(right_line_list2[X_Right_Down_Point], right_line_list2[X_Right_Up_Point], X_Right_Down_Point, X_Right_Up_Point, right);
    }
    else if (X_Left_Down_Point != 0 && X_Left_Up_Point != 0 && X_Right_Down_Point == 0 && X_Right_Up_Point != 0)//��б����
    {
        cross_buxian(left_line_list2[X_Left_Down_Point], left_line_list2[X_Left_Up_Point], X_Left_Down_Point, X_Left_Up_Point, left);
        Lengthen_Right_down(X_Right_Up_Point);
    }
    else if (X_Left_Down_Point != 0 && X_Left_Up_Point == 0 && X_Right_Down_Point != 0 && X_Right_Up_Point != 0)//��б����
    {
        cross_buxian(right_line_list2[X_Right_Down_Point], right_line_list2[X_Right_Up_Point], X_Right_Down_Point, X_Right_Up_Point, right);
        Lengthen_Left_up(X_Left_Down_Point);
    }
    else if (X_Left_Down_Point != 0 && X_Left_Up_Point != 0 && X_Right_Down_Point == 0 && X_Right_Up_Point == 0)
    {
        cross_buxian(left_line_list2[X_Left_Down_Point], left_line_list2[X_Left_Up_Point], X_Left_Down_Point, X_Left_Up_Point, left);
    }
    //    else if (X_Left_Down_Point == 0 && X_Left_Up_Point != 0 && X_Right_Down_Point == 0 && X_Right_Up_Point == 0)
    //{
    //    Lengthen_Left_down(X_Left_Up_Point);
    //}
    //else if (X_Left_Down_Point == 0 && X_Left_Up_Point == 0 && X_Right_Down_Point == 0 && X_Right_Up_Point != 0)
    //{
    //    Lengthen_Right_down(X_Right_Up_Point);
    //}
    else if (X_Left_Down_Point != 0 && X_Left_Up_Point == 0 && X_Right_Down_Point == 0 && X_Right_Up_Point == 0)//����
    {
        Lengthen_Left_up(X_Left_Down_Point);
    }
    else if (X_Left_Down_Point == 0 && X_Left_Up_Point == 0 && X_Right_Down_Point != 0 && X_Right_Up_Point == 0)//����
    {
        Lengthen_Right_up(X_Right_Down_Point);
    }
}


void Longest_White_Colum(void)
{
    memset(White_Column, 0, MT9V03X_W);
    memset(left_line_list2, 0, MT9V03X_H);
    memset(right_line_list2, MT9V03X_W-1, MT9V03X_H);
    memset(mid_line_list2, 0, MT9V03X_H);
    memset(Left_Lost_Flag, 0, MT9V03X_H);
    memset(Right_Lost_Flag, 0, MT9V03X_H);
    Left_Lost_Time = 0;                 //��߽綪�ߴ���
    Right_Lost_Time = 0;                //�ұ߽綪�ߴ���
    Both_Lost_Time = 0;                 //���߶��ߴ���
    Longest_White_Column_Left[0] = 0;//�����,[0]������еĳ��ȣ�[1���ǵ�ĳ��
    Longest_White_Column_Left[1] = 0;//�����,[0]������еĳ��ȣ�[1���ǵ�ĳ��
    Longest_White_Column_Right[0] = 0;//�����,[0]������еĳ��ȣ�[1���ǵ�ĳ��
    Longest_White_Column_Right[1] = 0;//�����,[0]������еĳ��ȣ�[1���ǵ�ĳ��
    Longest_White_Column_Final[0] = 0;
    Longest_White_Column_Final[1] = 0;
    int i, j;
    //�����ң��������ϣ�����ȫͼ��¼��Χ�ڵ�ÿһ�а׵�����
    for (j = find_error; j <= MT9V03X_W- find_error; j++)
    {
        for (i = MT9V03X_H - 1; i >= 0; i--)
        {
            if (image[i][j] == 0)
                break;
            else
                White_Column[j]++;
        }
    }
    if (right_search_flag==1)
    {
        for (i = right_start; i < MT9V03X_W - find_error; i++)//���������
        {
            if (Longest_White_Column_Left[0] < White_Column[i])//�������һ��
            {
                Longest_White_Column_Left[0] = White_Column[i];//��0���ǰ��г���
                Longest_White_Column_Left[1] = i;              //��1�����±꣬��j��
            }
        }
    }
    else
    {
        for (i = find_error; i <= MT9V03X_W - find_error; i++)//���������
        {
            if (Longest_White_Column_Left[0] < White_Column[i])//�������һ��
            {
                Longest_White_Column_Left[0] = White_Column[i];//��0���ǰ��г���
                Longest_White_Column_Left[1] = i;              //��1�����±꣬��j��
            }
        }
    }
    if (left_search_flag == 1)
    {
        for (i = left_start; i > find_error; i--)////���������
        {
            if (Longest_White_Column_Right[0] < White_Column[i])//�������һ��
            {
                Longest_White_Column_Right[0] = White_Column[i];//��0���ǰ��г���
                Longest_White_Column_Right[1] = i;              //��1�����±꣬��j��
            }
        }
    }
    else
    {
        for (i = MT9V03X_W - find_error; i >= find_error; i--)////���������
        {
            if (Longest_White_Column_Right[0] < White_Column[i])//�������һ��
            {
                Longest_White_Column_Right[0] = White_Column[i];//��0���ǰ��г���
                Longest_White_Column_Right[1] = i;              //��1�����±꣬��j��
            }
        }
    }
    if (right_search_flag == 1)
    {
        if (Longest_White_Column_Right[1] < Longest_White_Column_Left[1])
        {
            Longest_White_Column_Right[1] = Longest_White_Column_Left[1];
        }
    }
    else if (left_search_flag == 1)
    {
        if (Longest_White_Column_Right[1] < Longest_White_Column_Left[1])
        {
            Longest_White_Column_Left[1] =Longest_White_Column_Right[1];
        }
    }

    if(MT9V03X_W - 1-Longest_White_Column_Right[1]< Longest_White_Column_Left[1])
    {
        Longest_White_Column_Final[1] = Longest_White_Column_Right[1];
        Longest_White_Column_Final[0] = Longest_White_Column_Right[0];
    }
    else
    {
        Longest_White_Column_Final[1] = Longest_White_Column_Left[1];
        Longest_White_Column_Final[0] = Longest_White_Column_Left[0];
    }
    Search_Stop_Line = Longest_White_Column_Final[0];
    for (i = MT9V03X_H - 1; i >= MT9V03X_H - Search_Stop_Line; i--)
    {
        for (j = Longest_White_Column_Right[1]; j <= MT9V03X_W - 1 - 2; j++)
        {
            if (image[i][j] == IMG_WHITE && image[i][j + 1] == IMG_BLACK && image[i][j + 2] == IMG_BLACK)
            {
                right_line_list2[i] = j;
                Right_Lost_Flag[i] = 0;
                break;
            }
            else if (j >= MT9V03X_W - 1 - 2)
            {
                right_line_list2[i] = MT9V03X_W - 1;
                Right_Lost_Flag[i] = 1;
                break;
            }
        }
        for (j = Longest_White_Column_Left[1]; j >= 0 + 2; j--)
        {
            if (image[i][j] == IMG_WHITE && image[i][j - 1] == IMG_BLACK && image[i][j - 2] == IMG_BLACK)
            {
                left_line_list2[i] = j;
                Left_Lost_Flag[i] = 0;
                break;
            }
            else if (j <= 2)
            {
                left_line_list2[i] = 0;
                Left_Lost_Flag[i] = 1;
                break;
            }
        }

    }

    for (i = MT9V03X_H - 1; i >=MT9V03X_H - Search_Stop_Line; i--)
    {
        if (Right_Lost_Flag[i] == 1)
        {
            Right_Lost_Time++;
        }
        if (Left_Lost_Flag[i] == 1)
        {
            Left_Lost_Time++;
        }
        if (Right_Lost_Flag[i] == 1 && Left_Lost_Flag[i] == 1)
        {
            Both_Lost_Time++;
        }
        if (Boundry_Start_Right == 0 && Right_Lost_Flag[i] != 1)
        {
            Boundry_Start_Right = i;
        }
        if (Boundry_Start_Left == 0 && Left_Lost_Flag[i] != 1)
        {
            Boundry_Start_Left = i;
        }
    }
    if (Both_Lost_Time >= 5 && Both_Lost_Time <= MT9V03X_H - 10)//����ʮ��
    {
        Cross_Line();
    }
    if (Left_Lost_Time > Right_Lost_Time && Right_Lost_Time < 25 || Left_Lost_Time >= 50)
    {
        Right_Cross_line();
    }
    if (Right_Lost_Time > Left_Lost_Time && Left_Lost_Time < 25|| Right_Lost_Time>=50)
    {
        Left_Cross_line();
    }
    for (i = MT9V03X_H - 1; i >= MT9V03X_H - Search_Stop_Line; i--)
    {
        mid_line_list2[i] = (left_line_list2[i] + right_line_list2[i]) / 2;
    }
    for (i = MT9V03X_H - Search_Stop_Line-1; i > 0; i--)
    {
        mid_line_list2[i] = mid_line_list2[MT9V03X_H - Search_Stop_Line];
    }
}

void Show_Third_Line2(void)
{
    for(unsigned char i=Find_guai_point_start;i>Find_guai_point_end;i--)
    {
        if(image[i][mid_line_list2[i]]==255)
        {
            image[i][mid_line_list2[i]]=0;
        }
    }
    for(unsigned char i=Find_guai_point_start;i>Find_guai_point_end;i--)
    {
        if(image[i][left_line_list2[i]]==255)
        {
            image[i][left_line_list2[i]]=0;
        }
    }
    for(unsigned char i=Find_guai_point_start;i>Find_guai_point_end;i--)
    {
        if(image[i][right_line_list2[i]]==255)
        {
            image[i][right_line_list2[i]]=0;
        }
    }
}
