#ifndef UART_TXT_
#define UART_TXT_

#include "stdio.h"

volatile uint8_t rx_buf_cnt = 0;
volatile uint8_t rx_frame = 0;
volatile uint8_t rx_buf[16];

void IniteUART(uint32_t baudrate);
void Send_Char(char);
void USART_Send(volatile char *s);
void USART_Send_String(char * txt);
void USART_Send_Int(int16_t i);


void USART2_IRQHandler(void);

#endif //UART_TXT_
