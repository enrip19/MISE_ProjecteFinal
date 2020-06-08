/*
 * uart.h
 *
 *  Created on: 6 jun. 2020
 *      Author: enric
 */

#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "motorLib.h"

#ifndef LIBRARY_UARTLIB_H_
#define LIBRARY_UARTLIB_H_

//Variables
typedef unsigned char byte;
#define TXD0_READY (UCA0IFG & UCTXIFG)

uint8_t length;
uint16_t ucos16, ucbrf, ucbrs;


//Functions
void Init_UART(void);
void TXUAC0(byte TXData);
void send_UART(str_control *array);


#endif /* LIBRARY_UARTLIB_H_ */