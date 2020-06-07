/*
 * funcions.h
 *
 *  Created on: 23 abr. 2020
 *      Author: enric
 */

#ifndef FUNCIONS_H_
#define FUNCIONS_H_

#include <msp432p401r.h>
#include "stdio.h"
#include <stdint.h>

typedef struct {
    char array[20] ; //array 5 parametres + 3 maxim
}str_control;

str_control arrayCreator(uint8_t id, uint8_t length, uint8_t instruccio, uint8_t address, uint8_t *p_array);




#endif /* FUNCIONS_H_ */
