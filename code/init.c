/*
 * init.c
 *
 *  Created on: 2025年7月16日
 *      Author: interesting
 */
#include "init.h"
#define WIFI_SSID_TEST          "iQOO Neo9 Pro"
#define WIFI_PASSWORD_TEST      "@Wyl3516"  // 如果需要连接的WIFI 没有密码则需要将 这里 替换为 NULL
/*
 * 函数名：初始化
 * 输入参数：无
 * 输出参数：无
 * 作用：对所有有用的函数进行初始化
 * 例子：init_all();
 * 备注：放在while循环之前
 */
void init_all(void)
{

    pit_ms_init(CCU60_CH0, 5); //开启中断按键检测
    pit_ms_init(CCU60_CH1, 1000); //开启中断按键检测
    pit_disable(CCU60_CH1);
    pit_ms_init(CCU61_CH0,10);
    pit_disable(CCU61_CH0);
    oled_init();                //oled屏幕初始化
    Get_Num_All();              //读取Flash数据
    Servo_Init();               //舵机初始化
    Motor_Init();                 //电机初始化
/*********************wifi传输*************************************/
//    //等待蓝牙连接（耗时间，可以选择注释）
//        while(wifi_spi_init(WIFI_SSID_TEST, WIFI_PASSWORD_TEST))
//        {
//            printf("\r\n connect wifi failed. \r\n");
//            system_delay_ms(100);                                                   // 初始化失败 等待 100ms
//        }
//        if(1 != WIFI_SPI_AUTO_CONNECT)                                              // 如果没有开启自动连接 就需要手动连接目标 IP
//            {
//                while(wifi_spi_socket_connect(                                          // 向指定目标 IP 的端口建立 TCP 连接
//                    "TCP",                                                              // 指定使用TCP方式通讯
//                    WIFI_SPI_TARGET_IP,                                                 // 指定远端的IP地址，填写上位机的IP地址
//                    WIFI_SPI_TARGET_PORT,                                               // 指定远端的端口号，填写上位机的端口号，通常上位机默认是8080
//                    WIFI_SPI_LOCAL_PORT))                                               // 指定本机的端口号
//                {
//                    // 如果一直建立失败 考虑一下是不是没有接硬件复位
//                    printf("\r\n Connect TCP Servers error, try again.");
//                    system_delay_ms(100);                                               // 建立连接失败 等待 100ms
//                }
//            }
//    //传输图像的初始化
//    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);
//    seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_MT9V03X, base_image[0], MT9V03X_W, MT9V03X_H);
/**************************串口传输*************************************************/
// 设置逐飞助手使用DEBUG串口进行收发
//    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_DEBUG_UART);//串口助手发送图像初始化
//    seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_MT9V03X, base_image[0], MT9V03X_W, MT9V03X_H);
///***************************边线信息**************************************/
//    seekfree_assistant_camera_boundary_config(X_BOUNDARY, MT9V03X_H, left_line_list2, right_line_list2, mid_line_list2, NULL, NULL ,NULL);
//菜单的初始界面
    for(unsigned char i=0;i<max_OLED_ROW;i++)
    {
        oled_show_string(Show_String_x,(ROW_DISTANCE+1)*i,Pin->page[i]);
    }
    oled_show_string(Show_arrow_x,0,"<");
    system_delay_ms(500);
    mt9v03x_init();             //摄像头初始化
}


