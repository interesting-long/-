/*
 * image2.c
 *
 *  Created on: 2025年7月27日
 *      Author: interesting
 */

#include "image2.h"
unsigned char base_image[MT9V03X_H][MT9V03X_W]; //原始图像
unsigned char image[MT9V03X_H][MT9V03X_W];      //二值化之后的图像数组
unsigned char Final_mid_line=MID_W;

unsigned char left_line_list2[MT9V03X_H];        //左边线数组
unsigned char right_line_list2[MT9V03X_H];      //右边线数组
unsigned char mid_line_list2[MT9V03X_H];         //中间线数组
unsigned char vari_line_list[MT9V03X_H];                   //方差数组
unsigned char White_Column[MT9V03X_W];          //白点的分布直方图
unsigned char Left_Lost_Time = 0;                 //左边界丢线次数
unsigned char Right_Lost_Time = 0;                //右边界丢线次数
unsigned char Both_Lost_Time = 0;                 //两边丢线次数
unsigned char Search_Stop_Line;                 //停止查询行
unsigned char Longest_White_Column_Left[2];     //左最长白列的信息数组
unsigned char Longest_White_Column_Right[2];    //右最长白列的信息数组
unsigned char Longest_White_Column_Final[2];    //用于扫线的最长白列
unsigned char Left_Lost_Flag[MT9V03X_H];        //左边界丢线标志数组
unsigned char Right_Lost_Flag[MT9V03X_H];       //右边界丢线标志数组
unsigned char Boundry_Start_Left = 0;          //第一个非丢线点,常规左边界起始点
unsigned char Boundry_Start_Right = 0;          //第一个非丢线点,常规右边界起始点
//拐点
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
unsigned char otsuThreshold(unsigned char *image)   //注意计算阈值的一定要是原图像
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
    unsigned char* data = image;  //指向像素数据的指针
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }

    uint32 gray_sum=0;
    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i+=1)
    {
        for (j = 0; j < width; j+=1)
        {
            pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
            gray_sum+=(int)data[i * width + j];       //灰度值总和
            if(data[i * width + j]>Pixel_Max)   Pixel_Max=data[i * width + j];
            if(data[i * width + j]<Pixel_Min)   Pixel_Min=data[i * width + j];
        }
    }

    //计算每个像素值的点在整幅图像中的比例

    for (i = Pixel_Min; i < Pixel_Max; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;

    }

    //遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;

    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = Pixel_Min; j < Pixel_Max; j++)
    {

        w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值

        w1=1-w0;
        u1tmp=gray_sum/pixelSum-u0tmp;

        u0 = u0tmp / w0;              //背景平均灰度
        u1 = u1tmp / w1;              //前景平均灰度
        u = u0tmp + u1tmp;            //全局平均灰度
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
/*函数：将base_image按照传入的阈值，二值化之后放在image当中
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
 * 函数名：连线函数
 * 传入参数1：起始点的x坐标
 * 传入参数2：结束点的x坐标
 * 传入参数3：起始点的y坐标
 * 传入参数4：结束点的y坐标
* 传入参数5：数组类型
 *
 */
void cross_buxian(int x1, int x2, int y1, int y2,unsigned char num) {
    static float left_last_K = 0;
    static float right_last_K = 0;
    // 边界检查（保持不变）
    if (x1 >= MT9V03X_W - 1) x1 = MT9V03X_W - 1;
    else if (x1 < 0) x1 = 0; // 注意：原代码是 <=0，建议改为 <0 更合理

    if (y1 >= MT9V03X_H - 1) y1 = MT9V03X_H - 1;
    else if (y1 < 0) y1 = 0;

    if (x2 >= MT9V03X_W - 1) x2 = MT9V03X_W - 1;
    else if (x2 < 0) x2 = 0;

    if (y2 >= MT9V03X_H - 1) y2 = MT9V03X_H - 1;
    else if (y2 < 0) y2 = 0;

    int a1 = y1+1, a2 = y2-1;
    int b1 = x1+1, b2 = x2-1;

    // 确保 a1 <= a2（交换点坐标）
    if (a1 > a2) {
        int temp = a1; a1 = a2; a2 = temp;
        temp = b1; b1 = b2; b2 = temp;
    }

    // 处理垂直线或单点（y1 == y2）
    if (a1 == a2) {
        // 水平线：只需设置一个点（或整个线段横坐标相同）
        if (a1 >= 0 && a1 < MT9V03X_H) {
            // 取 b1 或 b2 均可，这里选 b1
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

    // 计算斜率（使用浮点避免截断）
    float K = (float)(b2 - b1) / (float)(a2 - a1); // 关键修改：浮点除法
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
        // 计算横坐标（四舍五入提高精度）
        int x = b1 + (int)((i - a1) * K + 0.5);

        // 约束横坐标到图像边界
        if (x < 0) x = 0;
        else if (x >= MT9V03X_W) x = MT9V03X_W - 1;

        // 存储到右边线数组
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
    if (start >= MT9V03X_H - 1 - 5)//下面5行上面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;//另一方面，当判断第i行时，会访问到i+3和i-4行，防止越界
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
    if (start >= MT9V03X_H - 1 - 5)//下面5行上面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;//另一方面，当判断第i行时，会访问到i+3和i-4行，防止越界
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
    if (start >= MT9V03X_H - 1 - 5)//下面5行上面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;//另一方面，当判断第i行时，会访问到i+3和i-4行，防止越界
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
    if (start >= MT9V03X_H - 1 - 5)//下面5行上面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;//另一方面，当判断第i行时，会访问到i+3和i-4行，防止越界
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
/************************斜入拐点判断*******下方*************************************/
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
    if (start >= MT9V03X_H - 1 - 5)//下面5行上面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;//另一方面，当判断第i行时，会访问到i+3和i-4行，防止越界
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
    if (start >= MT9V03X_H - 1 - 5)//下面5行上面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;//另一方面，当判断第i行时，会访问到i+3和i-4行，防止越界
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
    if (start >= MT9V03X_H - 1 - 5)//下面5行上面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;//另一方面，当判断第i行时，会访问到i+3和i-4行，防止越界
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
    if (start >= MT9V03X_H - 1 - 5)//下面5行上面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;//另一方面，当判断第i行时，会访问到i+3和i-4行，防止越界
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
/************************斜入拐点判断*******上方*************************************/
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
    if ((end - begin) * x2sum - xsum * xsum) //判断除数是否为零
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
    if (Left_Down_Point != 0 && Left_Up_Point != 0 && Right_Down_Point != 0 && Right_Up_Point != 0)//四个拐点：连线
    {
        cross_buxian(left_line_list2[Left_Down_Point], left_line_list2[Left_Up_Point], Left_Down_Point, Left_Up_Point, left);
        cross_buxian(right_line_list2[Right_Down_Point], right_line_list2[Right_Up_Point], Right_Down_Point, Right_Up_Point,right);
    }
    else if (Left_Down_Point != 0 && Left_Up_Point == 0 && Right_Down_Point != 0 && Right_Up_Point == 0)//只找到上拐点；延长上边线
    {
        Lengthen_Left_up(Left_Down_Point);
        Lengthen_Right_up(Right_Down_Point);
    }
    else if (Left_Down_Point == 0 && Left_Up_Point != 0 && Right_Down_Point == 0 && Right_Up_Point != 0)//只找到下拐点；延长下边线
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
    if (X_Left_Down_Point != 0 && X_Left_Up_Point != 0 && X_Right_Down_Point != 0 && X_Right_Up_Point != 0)//四个拐点：连线
    {
        cross_buxian(left_line_list2[X_Left_Down_Point], left_line_list2[X_Left_Up_Point], X_Left_Down_Point, X_Left_Up_Point, left);
        cross_buxian(right_line_list2[X_Right_Down_Point], right_line_list2[X_Right_Up_Point], X_Right_Down_Point, X_Right_Up_Point, right);
    }
    else if (X_Left_Down_Point == 0 && X_Left_Up_Point != 0 && X_Right_Down_Point != 0 && X_Right_Up_Point != 0)//右弯
    {
        cross_buxian(right_line_list2[X_Right_Down_Point], right_line_list2[X_Right_Up_Point], X_Right_Down_Point, X_Right_Up_Point, right);
        Lengthen_Left_down(X_Left_Up_Point);
    }
    else if (X_Left_Down_Point != 0 && X_Left_Up_Point != 0 && X_Right_Down_Point == 0 && X_Right_Up_Point != 0)//左斜入弯
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
    else if (X_Left_Down_Point != 0 && X_Left_Up_Point == 0 && X_Right_Down_Point == 0 && X_Right_Up_Point == 0)//右弯
    {
        Lengthen_Left_up(X_Left_Down_Point);
    }
    else if (X_Left_Down_Point == 0 && X_Left_Up_Point == 0 && X_Right_Down_Point != 0 && X_Right_Up_Point == 0)//右弯
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
    if (X_Left_Down_Point != 0 && X_Left_Up_Point != 0 && X_Right_Down_Point != 0 && X_Right_Up_Point != 0)//四个拐点：连线
    {
        cross_buxian(left_line_list2[X_Left_Down_Point], left_line_list2[X_Left_Up_Point], X_Left_Down_Point, X_Left_Up_Point, left);
        cross_buxian(right_line_list2[X_Right_Down_Point], right_line_list2[X_Right_Up_Point], X_Right_Down_Point, X_Right_Up_Point, right);
    }
    else if (X_Left_Down_Point != 0 && X_Left_Up_Point != 0 && X_Right_Down_Point == 0 && X_Right_Up_Point != 0)//左斜入弯
    {
        cross_buxian(left_line_list2[X_Left_Down_Point], left_line_list2[X_Left_Up_Point], X_Left_Down_Point, X_Left_Up_Point, left);
        Lengthen_Right_down(X_Right_Up_Point);
    }
    else if (X_Left_Down_Point != 0 && X_Left_Up_Point == 0 && X_Right_Down_Point != 0 && X_Right_Up_Point != 0)//左斜入弯
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
    else if (X_Left_Down_Point != 0 && X_Left_Up_Point == 0 && X_Right_Down_Point == 0 && X_Right_Up_Point == 0)//右弯
    {
        Lengthen_Left_up(X_Left_Down_Point);
    }
    else if (X_Left_Down_Point == 0 && X_Left_Up_Point == 0 && X_Right_Down_Point != 0 && X_Right_Up_Point == 0)//右弯
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
    Left_Lost_Time = 0;                 //左边界丢线次数
    Right_Lost_Time = 0;                //右边界丢线次数
    Both_Lost_Time = 0;                 //两边丢线次数
    Longest_White_Column_Left[0] = 0;//最长白列,[0]是最长白列的长度，[1】是第某列
    Longest_White_Column_Left[1] = 0;//最长白列,[0]是最长白列的长度，[1】是第某列
    Longest_White_Column_Right[0] = 0;//最长白列,[0]是最长白列的长度，[1】是第某列
    Longest_White_Column_Right[1] = 0;//最长白列,[0]是最长白列的长度，[1】是第某列
    Longest_White_Column_Final[0] = 0;
    Longest_White_Column_Final[1] = 0;
    int i, j;
    //从左到右，从下往上，遍历全图记录范围内的每一列白点数量
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
        for (i = right_start; i < MT9V03X_W - find_error; i++)//找左最长白列
        {
            if (Longest_White_Column_Left[0] < White_Column[i])//找最长的那一列
            {
                Longest_White_Column_Left[0] = White_Column[i];//【0】是白列长度
                Longest_White_Column_Left[1] = i;              //【1】是下标，第j列
            }
        }
    }
    else
    {
        for (i = find_error; i <= MT9V03X_W - find_error; i++)//找左最长白列
        {
            if (Longest_White_Column_Left[0] < White_Column[i])//找最长的那一列
            {
                Longest_White_Column_Left[0] = White_Column[i];//【0】是白列长度
                Longest_White_Column_Left[1] = i;              //【1】是下标，第j列
            }
        }
    }
    if (left_search_flag == 1)
    {
        for (i = left_start; i > find_error; i--)////找右最长白列
        {
            if (Longest_White_Column_Right[0] < White_Column[i])//找最长的那一列
            {
                Longest_White_Column_Right[0] = White_Column[i];//【0】是白列长度
                Longest_White_Column_Right[1] = i;              //【1】是下标，第j列
            }
        }
    }
    else
    {
        for (i = MT9V03X_W - find_error; i >= find_error; i--)////找右最长白列
        {
            if (Longest_White_Column_Right[0] < White_Column[i])//找最长的那一列
            {
                Longest_White_Column_Right[0] = White_Column[i];//【0】是白列长度
                Longest_White_Column_Right[1] = i;              //【1】是下标，第j列
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
    if (Both_Lost_Time >= 5 && Both_Lost_Time <= MT9V03X_H - 10)//正入十字
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
