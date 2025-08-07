/*
 * init.c
 *
 *  Created on: 2025��7��16��
 *      Author: interesting
 */
#include "init.h"
#define WIFI_SSID_TEST          "iQOO Neo9 Pro"
#define WIFI_PASSWORD_TEST      "@Wyl3516"  // �����Ҫ���ӵ�WIFI û����������Ҫ�� ���� �滻Ϊ NULL
/*
 * ����������ʼ��
 * �����������
 * �����������
 * ���ã����������õĺ������г�ʼ��
 * ���ӣ�init_all();
 * ��ע������whileѭ��֮ǰ
 */
void init_all(void)
{

    pit_ms_init(CCU60_CH0, 5); //�����жϰ������
    pit_ms_init(CCU60_CH1, 1000); //�����жϰ������
    pit_disable(CCU60_CH1);
    pit_ms_init(CCU61_CH0,10);
    pit_disable(CCU61_CH0);
    oled_init();                //oled��Ļ��ʼ��
    Get_Num_All();              //��ȡFlash����
    Servo_Init();               //�����ʼ��
    Motor_Init();                 //�����ʼ��
/*********************wifi����*************************************/
//    //�ȴ��������ӣ���ʱ�䣬����ѡ��ע�ͣ�
//        while(wifi_spi_init(WIFI_SSID_TEST, WIFI_PASSWORD_TEST))
//        {
//            printf("\r\n connect wifi failed. \r\n");
//            system_delay_ms(100);                                                   // ��ʼ��ʧ�� �ȴ� 100ms
//        }
//        if(1 != WIFI_SPI_AUTO_CONNECT)                                              // ���û�п����Զ����� ����Ҫ�ֶ�����Ŀ�� IP
//            {
//                while(wifi_spi_socket_connect(                                          // ��ָ��Ŀ�� IP �Ķ˿ڽ��� TCP ����
//                    "TCP",                                                              // ָ��ʹ��TCP��ʽͨѶ
//                    WIFI_SPI_TARGET_IP,                                                 // ָ��Զ�˵�IP��ַ����д��λ����IP��ַ
//                    WIFI_SPI_TARGET_PORT,                                               // ָ��Զ�˵Ķ˿ںţ���д��λ���Ķ˿ںţ�ͨ����λ��Ĭ����8080
//                    WIFI_SPI_LOCAL_PORT))                                               // ָ�������Ķ˿ں�
//                {
//                    // ���һֱ����ʧ�� ����һ���ǲ���û�н�Ӳ����λ
//                    printf("\r\n Connect TCP Servers error, try again.");
//                    system_delay_ms(100);                                               // ��������ʧ�� �ȴ� 100ms
//                }
//            }
//    //����ͼ��ĳ�ʼ��
//    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);
//    seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_MT9V03X, base_image[0], MT9V03X_W, MT9V03X_H);
/**************************���ڴ���*************************************************/
// �����������ʹ��DEBUG���ڽ����շ�
//    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_DEBUG_UART);//�������ַ���ͼ���ʼ��
//    seekfree_assistant_camera_information_config(SEEKFREE_ASSISTANT_MT9V03X, base_image[0], MT9V03X_W, MT9V03X_H);
///***************************������Ϣ**************************************/
//    seekfree_assistant_camera_boundary_config(X_BOUNDARY, MT9V03X_H, left_line_list2, right_line_list2, mid_line_list2, NULL, NULL ,NULL);
//�˵��ĳ�ʼ����
    for(unsigned char i=0;i<max_OLED_ROW;i++)
    {
        oled_show_string(Show_String_x,(ROW_DISTANCE+1)*i,Pin->page[i]);
    }
    oled_show_string(Show_arrow_x,0,"<");
    system_delay_ms(500);
    mt9v03x_init();             //����ͷ��ʼ��
}


