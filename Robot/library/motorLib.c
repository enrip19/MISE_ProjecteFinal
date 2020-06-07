/*
 * funcions.c
 *
 *  Created on: 23 abr. 2020
 *      Author: enric i sergio
 */
#include <msp432p401r.h>
#include <stdio.h>
#include <stdint.h>
#include "motorLib.h"
#include <string.h>


str_control arrayCreator(uint8_t id, uint8_t length, uint8_t instruccio, uint8_t address, uint8_t *p_array)
{
    str_control control;
    str_control *control_punter;
    control_punter = &control;

    memset(control.array,'0', 20); //tot a 0

    control_punter -> array[0]= 0xFF;
    control_punter -> array[1]= 0xFF;
    control_punter -> array[2]= id;
    control_punter -> array[3] = length;
    control_punter -> array[4] = instruccio;
    control_punter -> array[5] = address;


    //punter = &parametres; //Posem el punter apuntant al primer byte de parametres
    uint8_t i;
    for (i=0; i<(length-3); i++)
    {
        control_punter -> array[i+6] = *p_array;
        p_array++;
    }

    return *control_punter;
    //return 0;
}



