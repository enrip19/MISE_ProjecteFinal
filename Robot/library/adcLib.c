/*
 * adcLib.c
 *
 *  Created on: 9 may. 2020
 *      Author: enric
 */

#include "adcLib.h"
#include "miscLib.h"
#include "timerLib.h"
#include "lcdLib.h"
#include "motorLib.h"



void init_ADC(void){
    // Configure P5.4 (A1) i P5.5(A0) for ADC (com a secondary function)
    P5->SEL1 |= BIT4 | BIT5;
    P5->SEL0 |= BIT4 | BIT5;

   // sequence mode, no repeat, Sampling time, S&H=16, ADC14 on
    ADC14->CTL0 = ADC14_CTL0_CONSEQ_1 | ADC14_CTL0_SHT0_2 | ADC14_CTL0_SHP | ADC14_CTL0_ON;
    ADC14->CTL1 = ADC14_CTL1_RES_2;         // Use  sampling timer, 12-bit conversion results


    ADC14->MCTL[0] |= ADC14_MCTLN_INCH_0;   // A1 ADC input select for MEM0; Vref=AVCC
    ADC14->MCTL[1] |= ADC14_MCTLN_INCH_1 | ADC14_MCTLN_EOS;   // A0 ADC input select for MEM1; Vref=AVCC

    ADC14->IER0 = ADC14_IER0_IE1;          // Enable ADC conv complete interrupt (MEM0 o A1 i  MEM1 o A0)


}

uint16_t read_LDR(uint16_t newADC){

    uint16_t valueADC, newValue;

    newValue = (uint8_t) newADC;

    if(adcFlag==1){                                                 //si la interrupcio de ADC esta activa (per tant s'aixeca la flag)
        if(newValue >= 255){
            valueADC = 255;
        }
        else if(newValue <= 0){
            valueADC = 0;
        }
        else{
            valueADC = newValue;
        }
    }
    //printf("%d,%d\n",newValue, valueADC);
    return valueADC;
}


// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    if(ADC14->IFGR0 & ADC14_IFGR0_IFG1){
        ADC14->IER0 &= ~ADC14_IER0_IE1;
        newADCR = ADC14->MEM[1];
        newADCL = ADC14->MEM[0];
        adcFlag = 1;
    }

}
