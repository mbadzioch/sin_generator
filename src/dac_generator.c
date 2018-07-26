/*
 * dac_generator.c
 *
 *  Created on: 17.07.2018
 *      Author: MCV
 */
#include "dac_generator.h"
#include "stm32f0xx_conf.h"
#include "stm32f0xx_spi.h"

#define SPI1_SCK GPIO_Pin_5
#define SPI1_MISO GPIO_Pin_6
#define SPI1_MOSI GPIO_Pin_7
#define LDAC GPIO_Pin_4 	//#LDAC = 0
#define CS GPIO_Pin_3  		//#CS = 1

static void DAC_Inite(void);
static void SPI_Inite(void);
static void SPI_PinInit(void);




void Init_dac_generator(void){
	SPI_PinInit();
	SPI_Inite();
	DAC_Inite();
}

void Spi_Send_Ampl(uint8_t ampl)
{
	uint16_t data=0;
	lock_spi = 1;

	data = 0x3000 | (ampl << 4);				 // DACa ENABLE


    GPIO_ResetBits(GPIOA, CS);  // DAC #CS = 0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData16(SPI1, data);
//	    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==0);
//	    SPI_I2S_ReceiveData(SPI1);
//	    SPI_I2S_SendData(SPI1, 0xff);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    data = SPI_I2S_ReceiveData(SPI1);
    GPIO_SetBits(GPIOA, CS);	// DAC #CS = 1;

    lock_spi = 0;
}

void Spi_SetFreq(uint16_t freq){
	uint16_t temp=0;


	temp = 0xB000 | (freq << 4);			       // DACb ENABLE

    GPIO_ResetBits(GPIOA, CS);  // DAC #CS = 0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData16(SPI1, temp);
//	    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==0);
//	    SPI_I2S_ReceiveData(SPI1);
//	    SPI_I2S_SendData(SPI1, 0xff);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    temp = SPI_I2S_ReceiveData(SPI1);
    GPIO_SetBits(GPIOA, CS);	// DAC #CS = 1;

}

void Spi_Send(uint8_t reg, uint8_t data)
{
	uint16_t temp;

	temp = reg;
	temp = temp << 8;
	temp |= data;
	temp &= ~0x8000;

    GPIO_ResetBits(GPIOE, GPIO_Pin_3);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData16(SPI1, temp);
//	    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==0);
//	    SPI_I2S_ReceiveData(SPI1);
//	    SPI_I2S_SendData(SPI1, 0xff);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    SPI_I2S_ReceiveData(SPI1);
    GPIO_SetBits(GPIOE, GPIO_Pin_3);
}

static void SPI_PinInit(void){
	GPIO_InitTypeDef GPIO_InitStruct;


	// Enable clock for USART2 peripheral
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	// Enable clock for GPIOA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);


	// Initialize pins as alternating function
	GPIO_InitStruct.GPIO_Pin = SPI1_SCK | SPI1_MOSI | SPI1_MISO;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_0);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_0);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_0);
}

static void SPI_Inite(void){
	SPI_InitTypeDef I2S_InitStruct;

	// Enable clock for USART2 peripheral
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	SPI_I2S_DeInit(SPI1);
	SPI_StructInit(&I2S_InitStruct);
	I2S_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	I2S_InitStruct.SPI_DataSize = SPI_DataSize_16b;
	I2S_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	I2S_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	I2S_InitStruct.SPI_NSS = SPI_NSS_Soft;
	I2S_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	I2S_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	I2S_InitStruct.SPI_CRCPolynomial = 7;

	I2S_InitStruct.SPI_Mode = SPI_Mode_Master;

	/* Enable the SPI peripheral */
	SPI_Cmd(SPI1, ENABLE);
}


//********* INICJALIZACJA DAC *************************
static void DAC_Inite(void){
	//Spi_Send(0x20,0x47);

}
