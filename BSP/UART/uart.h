/**
 ****************************************************************************************************
 * @file        uart.h
 * @version     V1.0
 * @date        2025-01-13
 * @brief       串口驱动代码
 ****************************************************************************************************
 * @attention
 * 
 * 
 ****************************************************************************************************
 */

#ifndef __UART_H
#define __UART_H

#include "main.h"
#include <stdio.h>

/* 硬件定义 */
#define UART_UX         USART3

/* 功能定义 */
#ifndef BSP_UART_RX_DISABLE
#define UART_EN_RX      1                   /* 使能串口接收功能 */
#else
#define UART_EN_RX      0                   /* 禁用串口接收功能 */
#endif
#define RXBUFFERSIZE    1                   /* 串口中断接收缓冲区大小 */
#define UART_REC_LEN    200                 /* 串口接收缓冲区大小 */

/* 变量导出 */
#if UART_EN_RX
extern uint8_t g_rx_buffer[RXBUFFERSIZE];   /* 串口中断接收缓冲区 */
extern uint8_t g_uart_rx_buf[UART_REC_LEN]; /* 串口接收缓冲区 */
extern uint16_t g_uart_rx_sta;              /* 串口接收状态标记 */
#endif

/* 函数声明 */
void uart_init(uint32_t baudrate);             /* 初始化串口 */

#endif
