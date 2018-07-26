/*
 * dac_generator.h
 *
 *  Created on: 17.07.2018
 *      Author: MCV
 */

#ifndef DAC_GENERATOR_H_
#define DAC_GENERATOR_H_

#include "stdio.h"

volatile uint8_t lock_spi;

void Init_dac_generator(void);
void Spi_Send_Ampl(uint8_t ampl);
void Spi_SetFreq(uint16_t freq);
void Spi_Send(uint8_t reg, uint8_t data);

void Init_dac_generator(void);

#endif /* DAC_GENERATOR_H_ */
