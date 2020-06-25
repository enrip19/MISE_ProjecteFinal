/*
 * modesLib.h
 *
 *  Created on: 12 jun. 2020
 *      Author: enric
 */

#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


#ifndef LIBRARY_MODESLIB_H_
#define LIBRARY_MODESLIB_H_
#define sensibilidad 30

float velButtonF;
int velButton;
float girButtonF;
int girButton;
bool sentitButton;
uint8_t modeSel;

void buttonMode(void);
void autopilot (uint8_t left, uint8_t right );


#endif /* LIBRARY_MODESLIB_H_ */
