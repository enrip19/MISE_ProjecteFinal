/*
 * timerLib.c
 *
 *  Created on: 9 may. 2020
 *      Author: enric
 */

#include "timerLib.h"

uint16_t count;

void init_TA0(void){
    TA0CTL|= TASSEL__SMCLK | TIMER_A_CTL_IE;                         //Activar el clock SMCLK, activa las interrupciones

    TA0CCTL0|= CCIE;                                                //habilitamos las interrupciones del CCR0
    TA0CCR0=12000;                                                     //utilizamos un CCR de 1000 para tener una sensibilidad de ms (1 msec ara mateix)



}

void init_TA1(uint16_t ccr0Max, uint16_t ccr1){
    TA1CTL |= TASSEL__SMCLK | MC__UP | TIMER_A_CTL_IE;               //activa el clock SMCLK, activa las interrupciones, modo up

    TA1CCTL0 |= CCIE;                                               //habilitamos las interrupciones del CCR0
    TA1CCTL1 |= OUTMOD_3 | CCIE;                                    //habilitem mode toggle i les interrupcions del CCR1

    TA1CCR0 = ccr0Max;                                                  //periode del PWM
    TA1CCR1 = ccr1;                                                  //DUTY CYCLE (50%)


}

void delay(uint16_t temps_ms){
       count=0;                                                     //resetejem comptador
       TIMER_A0->CTL |= TACLR;                                      //fem un clear del ccr0 per assegurar que iniciem des de 0
       TIMER_A0->CTL |= MC__UP;                                     //Activem el mode UP per comencar a comptar

       while (count<temps_ms)                                       //Esperem a que el count arribi a les iteracions demanades (en aquest cas milisec's)
       {
       }
       TIMER_A0->CTL |= MC__STOP;                                   //Posem el timer en mode stop per a que no conti si no demanem la funcio
}

/* ISR del TimerA0 */
void TA0_0_IRQHandler(void) {
    TA0CCTL0&=~CCIE;                                                //deshabilitem les interrupcions de CCR0
    count++;                                                        //afegim una iteracio al contador
    TA0CCTL0|=CCIE;                                                 //Tornem ha habilitar les interrupcions de CCR0
    TA0CCTL0&=~CCIFG;                                               //Fem un clear a les flags d'interrupcio
}
