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

/*Variables*/
//TX
#define TXD0_READY (UCA0IFG & UCTXIFG)
typedef unsigned char byte;
//motors TX
uint16_t ucos16, ucbrf, ucbrs;

//RX
bool RXFlag;
uint8_t RXData;


//Functions
void Init_UART(void);
void TXUAC0(byte TXData);
void send_UART(str_control *array);


#endif /* LIBRARY_UARTLIB_H_ */
