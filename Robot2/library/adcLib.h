/*
 * adcLib.h
 *
 *  Created on: 9 may. 2020
 *      Author: enric
 */
#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef LIBRARY_ADCLIB_H_
#define LIBRARY_ADCLIB_H_

bool adcFlagL, adcFlagR, adcFlag;

uint16_t newADCL, newADCR;
uint8_t adcR, adcL, oldadcR, oldadcL;

void init_ADC(void);
uint16_t read_LDR(uint16_t newADC);


#endif /* LIBRARY_ADCLIB_H_ */
