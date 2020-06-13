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

float velButtonF;
int velButton;
float girButtonF;
int girButton;
bool sentitButton;

void buttonMode(void);

#endif /* LIBRARY_MODESLIB_H_ */
