/*
 * adcLib.c
 *
 *  Created on: 9 may. 2020
 *      Author: enric
 */

#include "adcLib.h"



void  (void){
    // Configure P5.4 for ADC (com a secondary function)
    P5->SEL1 |= BIT4;
    P5->SEL0 |= BIT4;

   // Sampling time, S&H=16, ADC14 on
    ADC14->CTL0 = ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
    ADC14->CTL1 = ADC14_CTL1_RES_2;         // Use sampling timer, 12-bit conversion results

    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_1;   // A1 ADC input select; Vref=AVCC
    ADC14->IER0 |= ADC14_IER0_IE0;          // Enable ADC conv complete interrupt


}


// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    uint8_t status = ADC14->IV;
    ADC14->IER0 &= ~ADC14_IER0_IE0;
    newADC = ADC14->MEM[0];
    adcFlag = 1;

}
