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

//defines de les instruccio dels motors
#define WRITE_MOT 0x03

//defines de les adreces dels motors
#define LDR_ADR 0x2D


//variables i structs
typedef struct {
    char array[20] ; //array 5 parametres + 3 maxim
}str_control;


char param [] = {};

//funcions
uint8_t CheckSum;
uint8_t checkSum_Calc(uint8_t id, uint8_t length, uint8_t instruccio, uint8_t address, uint8_t *p_array);
str_control arrayCreator(uint8_t id, uint8_t length, uint8_t instruccio, uint8_t address, uint8_t *p_array, uint8_t checkSum);

void send_Motor(uint8_t id, uint8_t length, uint8_t instruccio, uint8_t address, uint8_t *p_array);

void motorWrite_LDR(uint8_t id, uint8_t value);


#endif /* FUNCIONS_H_ */
