#include "uart.h"
#include "stm32f0xx.h"
#include <stm32f0xx_usart.h>
#include "string.h"
#include "stm32f0xx_conf.h"
#include "stdio.h"
#include "stdlib.h"


#define DEBUG_BUF_LENGTH 50

char bufor[50]={0};



void Send_Char(char c){
	//Sprawdza czy bufor nadawczy jest pusty
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, c);
}

void USART_Send_String(char *txt){
	while( *txt ) Send_Char(*txt++);
}


void USART_Send_Int(uint16_t i){
	char bufor[50];
	USART_Send_String( itoa(i, bufor, 10) );
}


void USART_Send(volatile char *s)
{
	while(*s){
		// wait until data register is empty
		while( !(USART1->TDR & 0x00000040) );
		USART_SendData(USART1, *s);
		s++;
	}
}



/*void UARTInit (uint32_t baudrate) {

    GPIOB->MODER &= ~(3 << (2 * 6)); // PB6 - RXD pin
    GPIOB->MODER |= (2 << (2 * 6)); //PB6 - UART1 TXD pin - AF mode
    GPIOB->OSPEEDR |= (3 << (2 * 6));//PA9 - high speed mode
    
    GPIOB->MODER &= ~(3 << (2 * 7)); //PB7 - TXD pin
    GPIOB->MODER |= (2 << (2 * 7)); //PB7 - USART1 RXD pin - AF mode
    GPIOB->OSPEEDR |= (3 << (2 * 7)); //PB7 - high speed mode
    
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; //enable USART1 clock
    
    USART1->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_TE; //enable USART
    USART1->CR1 |= USART_CR1_RXNEIE; // enable receive interrupt
    
    USART1->BRR = 48000000 / baudrate;
    
    rx_frame = 0;
    rx_buf_cnt = 0;
    

}*/

void UARTInite(uint32_t baudrate){
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;

	//Enable clock for USART1 peripheral
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	// Enable clock for GPIOA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	// Initialize pins as alternating function
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_0);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_0);

	/**
	 * Set Baudrate to value you pass to function
	 * Disable Hardware Flow control
	 * Set Mode To TX and RX, so USART will work in full-duplex mode
	 * Disable parity bit
	 * Set 1 stop bit
	 * Set Data bits to 8
	 *
	 * Initialize USART1
	 * Activate USART1
	 */
	USART_DeInit(USART1);

	USART_StructInit(&USART_InitStruct);
	USART_InitStruct.USART_BaudRate = baudrate;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStruct);
	USART_Cmd(USART1, ENABLE);

	// Initialize NVIC
	//NVIC_SetPriority(USART1_IRQn,0);
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
	NVIC_Init(&NVIC_InitStruct);

	//Enable RX interrupt
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);


}

/*void USART1_IRQHandler (void) {
    unsigned char  t_i;
    if ((USART1->ISR) & USART_ISR_RXNE) { //receved char
	t_i = USART1->RDR;
	if ((t_i != '\n') && (t_i != '\r')) { // normal char
	    rx_buf[rx_buf_cnt++] = (uint8_t) t_i;
	     if (rx_buf_cnt > 15) rx_buf_cnt = 15;
	} else { // EOL
	    rx_frame = 1;
	}
    }

    if ((USART1->ISR) & USART_ISR_ORE)  // USART1 overrun
	USART1->ICR |= USART_ICR_ORECF;
}*/



void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET){
		if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE)){
			if(rx_buf_cnt < DEBUG_BUF_LENGTH){
				rx_buf[rx_buf_cnt] = USART_ReceiveData(USART2);
				if(rx_buf[rx_buf_cnt] == '\r'){
					USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
				}
				else{
					rx_buf_cnt++;
					rx_frame = 1;
				}
			}
			else{
				USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
			}
		}
	}
}
