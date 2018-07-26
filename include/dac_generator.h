/*
 * dac_generator.h
 *
 *  Created on: 17.07.2018
 *      Author: MCV
 */

#ifndef DAC_GENERATOR_H_
#define DAC_GENERATOR_H_

#include "stdio.h"

void Init_dac_generator(void);
int8_t Spi_Send_Ampl(uint8_t ampl);
void Spi_Send(uint8_t reg, uint8_t data);

void Init_dac_generator(void);

#endif /* DAC_GENERATOR_H_ */
