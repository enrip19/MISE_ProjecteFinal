/*
 * miscLib.h
 *
 *  Created on: 9 may. 2020
 *      Author: enric
 */

#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef LIBRARY_MISCLIB_H_
#define LIBRARY_MISCLIB_H_

uint8_t buttonLFlag, buttonRFlag, buttonStatus;
uint8_t controlFlag;
uint8_t init_ucs_12MHz();
void init_GPIO(void);
void init_NVIC(void);
void init_portMapping(void);

#endif /* LIBRARY_MISCLIB_H_ */
