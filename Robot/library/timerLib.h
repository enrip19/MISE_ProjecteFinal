/*
 * timerLib.h
 *
 *  Created on: 9 may. 2020
 *      Author: enric
 */
#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef LIBRARY_TIMERLIB_H_
#define LIBRARY_TIMERLIB_H_

uint16_t pwm, pwmMax;

void init_TA0(void);
void init_TA1(uint16_t ccr0Max, uint16_t ccr1);
void delay(uint16_t temps_ms);

#endif /* LIBRARY_TIMERLIB_H_ */
