/*********************************************************************************************************************
* TC264 Opensourec Library 即（TC264 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 TC264 开源库的一部分
*
* TC264 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          cpu1_main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.10.2
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"
#pragma section all "cpu1_dsram"
                         // 默认 UART0_RX_P14_1

// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU1的RAM中
int Time;
unsigned char Stop_Flag;
unsigned char jiansu_mid_line;
// **************************** 代码区域 ****************************

// 本例程是开源库空工程 可用作移植或者测试各类内外设
// 本例程是开源库空工程 可用作移植或者测试各类内外设
// 本例程是开源库空工程 可用作移植或者测试各类内外设

void core1_main(void)
{
    disable_Watchdog();                     // 关闭看门狗
    interrupt_global_enable(0);             // 打开全局中断
    // 此处编写用户代码 例如外设初始化代码等
    init_all();
    uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN, UART_RX_PIN);                              // 输出换行
    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();                 // 等待所有核心初始化完毕
    while (TRUE)
    {
        // 此处编写需要循环执行的代码
        if(Show_image_mode==1)                                     //oled显示二值化图像和参考黑线并且进行运动
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
//            Servo_Pid_Update();                 //PID定性更新
            while(Show_image_mode==1)
            {
//                pit_disable(CCU60_CH0);
                if(mt9v03x_finish_flag==1 )
                {

                    /**************直弯参数分离*****************************/

                    Servo_Pid_Update(Final_mid_line,ser_yu);            //PID调节
                    Motor_Update();                                     //速度调节
                    Servo_turn_pid(Final_mid_line);                     //舵机PID打脚

                    /****************图像处理*******************/

                    memcpy(base_image[0], mt9v03x_image[0], MT9V03X_IMAGE_SIZE);
                    yuzhi=otsuThreshold(base_image[0]);
                    Set_image_twovalues(yuzhi);
                    Longest_White_Colum();
                    Fang_Error=Distinguish_straight_Turn((unsigned char)duqu_line-10,(unsigned char)duqu_line+30);
//                    Send_image(yuzhi);//Cute上位机发送图像
                    if(Protect_control())                        //Protect大于10触发保护；小于10关闭保护
                    {
                        Final_mid_line=mid_line_list2[(unsigned char)duqu_line];
                        jiansu_mid_line=mid_line_list2[5];
                        /**********OLED显示*************/
//                            Show_Third_Line2();
//                            oled_displayimage03x(image[0],0);//显示二值化
                        /**********显示具体参数*************/
                        Show_parameter();
                        /*******逐飞助手显示图像*********/
//                        Send_image_seekfree_assistant();            //在逐飞助手显示图像和三条参考线
                        /*******************************/
                    }
                    else            //保护暂停
                    {
                        Over_Stop_ALL_Control();
                    }
                    mt9v03x_finish_flag=0;
                }
                else if(Stop_Flag==0)  //时间暂停
                {
                    Time_Stop_ALL_Control();
                }
            }
            oled_clear();
        }
        else  if(Show_image_mode==0)                                    //菜单界面
        {
            MotorL_SetSpeed(0);//调参暂停
            MotorR_SetSpeed(0);
            Ser_Servo_Duty(duoji_mid);
            while(Show_image_mode==0)
            {
                if(Key)
                {
                    menu_handle_key(Key);
                    Key=0;              //!!!!!修改项，避免按下一瞬间判断反复触发
                }
            }
        }

        // 此处编写需要循环执行的代码
    }
}
IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    pit_clear_flag(CCU60_CH0);
    Key_scaner();
}
IFX_INTERRUPT(cc60_pit_ch1_isr, 0, CCU6_0_CH1_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
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
    interrupt_global_enable(0);                     // 开启中断嵌套
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
// **************************** 代码区域 ****************************
