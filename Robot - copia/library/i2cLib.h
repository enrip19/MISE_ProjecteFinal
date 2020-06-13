/*
 * i2cLib.h
 *
 *  Created on: 22 may. 2020
 *      Author: enric
 */
#ifndef LIBRARY_I2CLIB_H_
#define LIBRARY_I2CLIB_H_

#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

uint8_t *PTxData;                                                       // Pointer to TX data
uint8_t TXByteCtr;
uint8_t *PRxData;                                                       // Pointer to RX data
uint8_t RXByteCtr;

void init_I2C (void);
void I2C_send (uint8_t addr, uint8_t *buffer, uint8_t n_dades );
void I2C_receive(uint8_t addr, uint8_t *buffer, uint8_t n_dades);

#endif /* LIBRARY_I2CLIB_H_ */
