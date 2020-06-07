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

uint8_t adcFlag, sensibilitat;
uint16_t newADC, oldADC;

void init_ADC(void);

#endif /* LIBRARY_ADCLIB_H_ */
