/*
 * live.h
 *
 *  Created on: 2025��7��29��
 *      Author: interesting
 */

#ifndef CODE_LIVE_H_
#define CODE_LIVE_H_
#define UART_INDEX              (DEBUG_UART_INDEX   )                           // Ĭ�� UART_0
#define UART_BAUDRATE           (DEBUG_UART_BAUDRATE)                           // Ĭ�� 115200
#define UART_TX_PIN             (DEBUG_UART_TX_PIN  )                           // Ĭ�� UART0_TX_P14_0
#include "zf_common_headfile.h"

#define UART_RX_PIN             (DEBUG_UART_RX_PIN  )
void Send(unsigned char *data,unsigned int len);
void Send_image(unsigned char yuzhi);

#endif /* CODE_LIVE_H_ */
