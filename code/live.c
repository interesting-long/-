/*
 * live.c
 *
 *  Created on: 2025��7��29��
 *      Author: interesting
 */

#include "live.h"
// unsigned char Data[MT9V03X_H*MT9V03X_W/8];
uint8 uart_get_data[64];                                                        // ���ڽ������ݻ�����
uint8 fifo_get_data[64];                                                        // fifo �������������
uint8  get_data = 0;                                                            // �������ݱ���
uint32 fifo_data_count = 0;                                                     // fifo ���ݸ���
//fifo_init(&uart_data_fifo, FIFO_DATA_8BIT, uart_get_data, 64);              // ��ʼ�� fifo ���ػ�����

//uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN, UART_RX_PIN);             // ��ʼ������
//uart_write_string(UART_INDEX, "UART Text.");                                // ���������Ϣ
//uart_write_byte(UART_INDEX, '\r');                                          // ����س�
//uart_write_byte(UART_INDEX, '\n');
//fifo_struct uart_data_fifo;

unsigned char Data[MT9V03X_H*MT9V03X_W];
void Send(unsigned char *data,unsigned int len)
{
    for(int i=0;i<len;i++)
    {
        uart_write_byte(UART_INDEX,*(data+i));
    }
}
void Send_image(unsigned char yuzhi)
{
//    //֡ͷ
//    unsigned char header[7];
//    header[0]=0x28;         // ֡ͷ
//    header[1]=0x22;         // ����
//    unsigned int width=184,height=70;
//    memcpy(header+2,&width,2);
//    memcpy(header+4,&height,2);
//    header[6] = 0x2D;       //
//    Send(header,7);
//    //���ݶ�       128�������ֵ   Image��ԭʼ�Ҷ�ͼ��
//    for(int i = 0;i<184*70;i++)
//    {
//        if(*((unsigned char *)base_image + i) > yuzhi)
//        {
//            // Data[i/8] |= 1 << (i % 8);
//            Data[i] = 255;
//            uart_write_byte(UART_INDEX, 255);
//        }
//        else {
//            Data[i] = 0;
//            uart_write_byte(UART_INDEX, 0);
//        }
//
//    }
//    //Send((unsigned char *) Data,184*70);
//    //֡β
////    unsigned char tail = 0x29;
////    Send(&tail,1);
//    uart_write_byte(UART_INDEX, 0x29);

    //֡ͷ
    unsigned char header[7];
    header[0]=0x28;         // ֡ͷ
    header[1]=0x22;         // ����
    unsigned int width=184,height=70;
    memcpy(header+2,&width,2);
    memcpy(header+4,&height,2);
    header[6] = 0x2D;       //
    uart_write_buffer(UART_INDEX, header, 7);

    for(int i = 0;i < 184*70;i++)
    {
        if(*((unsigned char *)base_image + i) > yuzhi)
        {
            Data[i] = 255;
        }
        else {
            Data[i] = 0;
        }

    }

    uart_write_buffer(UART_INDEX, (unsigned char *)Data, 184*70);
    uart_write_byte(UART_INDEX, 0x29);

}

