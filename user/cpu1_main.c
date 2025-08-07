/*********************************************************************************************************************
* TC264 Opensourec Library ����TC264 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ��� TC264 ��Դ���һ����
*
* TC264 ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          cpu1_main
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          ADS v1.10.2
* ����ƽ̨          TC264D
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2022-09-15       pudding            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"
#pragma section all "cpu1_dsram"
                         // Ĭ�� UART0_RX_P14_1

// ���������#pragma section all restore���֮���ȫ�ֱ���������CPU1��RAM��
int Time;
unsigned char Stop_Flag;
unsigned char jiansu_mid_line;
// **************************** �������� ****************************

// �������ǿ�Դ��չ��� ��������ֲ���߲��Ը���������
// �������ǿ�Դ��չ��� ��������ֲ���߲��Ը���������
// �������ǿ�Դ��չ��� ��������ֲ���߲��Ը���������

void core1_main(void)
{
    disable_Watchdog();                     // �رտ��Ź�
    interrupt_global_enable(0);             // ��ȫ���ж�
    // �˴���д�û����� ���������ʼ�������
    init_all();
    uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN, UART_RX_PIN);                              // �������
    // �˴���д�û����� ���������ʼ�������
    cpu_wait_event_ready();                 // �ȴ����к��ĳ�ʼ�����
    while (TRUE)
    {
        // �˴���д��Ҫѭ��ִ�еĴ���
        if(Show_image_mode==1)                                     //oled��ʾ��ֵ��ͼ��Ͳο����߲��ҽ����˶�
        {
            oled_clear();
            oled_show_string(23,3,"Wait 1 second");
            system_delay_ms(1000);
            oled_clear();
            Ser_Servo_Duty(duoji_mid);
            Time=0;
            speed_time=0;
            wan_time=0;
            pit_enable(CCU60_CH1);
            pit_enable(CCU61_CH0);
            Stop_Flag=1;
//            Servo_Pid_Update();                 //PID���Ը���
            while(Show_image_mode==1)
            {
//                pit_disable(CCU60_CH0);
                if(mt9v03x_finish_flag==1 )
                {

                    /**************ֱ���������*****************************/

                    Servo_Pid_Update(Final_mid_line,ser_yu);            //PID����
                    Motor_Update();                                     //�ٶȵ���
                    Servo_turn_pid(Final_mid_line);                     //���PID���

                    /****************ͼ����*******************/

                    memcpy(base_image[0], mt9v03x_image[0], MT9V03X_IMAGE_SIZE);
                    yuzhi=otsuThreshold(base_image[0]);
                    Set_image_twovalues(yuzhi);
                    Longest_White_Colum();
                    Fang_Error=Distinguish_straight_Turn((unsigned char)duqu_line-10,(unsigned char)duqu_line+30);
//                    Send_image(yuzhi);//Cute��λ������ͼ��
                    if(Protect_control())                        //Protect����10����������С��10�رձ���
                    {
                        Final_mid_line=mid_line_list2[(unsigned char)duqu_line];
                        jiansu_mid_line=mid_line_list2[5];
                        /**********OLED��ʾ*************/
//                            Show_Third_Line2();
//                            oled_displayimage03x(image[0],0);//��ʾ��ֵ��
                        /**********��ʾ�������*************/
                        Show_parameter();
                        /*******���������ʾͼ��*********/
//                        Send_image_seekfree_assistant();            //�����������ʾͼ��������ο���
                        /*******************************/
                    }
                    else            //������ͣ
                    {
                        Over_Stop_ALL_Control();
                    }
                    mt9v03x_finish_flag=0;
                }
                else if(Stop_Flag==0)  //ʱ����ͣ
                {
                    Time_Stop_ALL_Control();
                }
            }
            oled_clear();
        }
        else  if(Show_image_mode==0)                                    //�˵�����
        {
            MotorL_SetSpeed(0);//������ͣ
            MotorR_SetSpeed(0);
            Ser_Servo_Duty(duoji_mid);
            while(Show_image_mode==0)
            {
                if(Key)
                {
                    menu_handle_key(Key);
                    Key=0;              //!!!!!�޸�����ⰴ��һ˲���жϷ�������
                }
            }
        }

        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}
IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    pit_clear_flag(CCU60_CH0);
    Key_scaner();
}
IFX_INTERRUPT(cc60_pit_ch1_isr, 0, CCU6_0_CH1_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    pit_clear_flag(CCU60_CH1);
    Time++;
    if(Time>=M_Time)
    {
        Time=0;
        Stop_Flag=0;
    }
}

IFX_INTERRUPT(cc61_pit_ch0_isr, 0, CCU6_1_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    pit_clear_flag(CCU61_CH0);
    if(abs(MID_W-jiansu_mid_line)<speed_err)
    {
        last_speed_time=speed_time;
        speed_time++;
        wan_time=0;
    }
    else if(abs(MID_W-jiansu_mid_line)>=speed_err)
    {
        wan_time++;
        speed_time=0;
    }

}
#pragma section all restore
// **************************** �������� ****************************
