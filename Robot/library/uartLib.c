/*
 * uart.c
 *
 *  Created on: 6 jun. 2020
 *      Author: enric
 */
#include "uartLib.h"

void Init_UART(void)
{
    UCA0CTL0 |= UCSWRST; //Fem un reset de la USCI i que estigui inoperativa
    UCA0CTL1 = 0x00; //UCSYNC=0 mode asíncron
                        //UCMODEx=0 seleccionem mode UART
                        //UCSPB=0 nomes 1 stop bit
                        //UC7BIT=0 8 bits de dades
                        //UCMSB=0 bit de menys pes primer
                        //UCPAR=x no es fa servir bit de paritat
                        //UCPEN=0 sense bit de paritat
    UCA0CTL0 |= UCSSEL__SMCLK; //Triem SMCLK (16MHz) com a font del clock BRCLK

    //Baud rate config (115200)
    UCA0BRW = 6;
    ucos16 = 1;
    ucbrf = 8; // posem valor a ucbrf
    ucbrf <<= 4; //el desplacem 4 bits per posar-lo al lloc que toca
    ucbrs = 0x20;
    ucbrs <<= 8;

    UCA0MCTLW |= (ucos16 | ucbrf | ucbrs);

    P1SEL0 |= (BIT2 | BIT3); //   solucion libro P1SEL1.x:P1SEL0.x=01

    P1SEL1 = 0; //I/O funció: P1.2 = UARTRX, P1.3 = UARTTX
    P1REN |= BIT2; //amb resistència activada de pull-up l’entrada P4.52
    P1OUT |= BIT2; //
    P1DIR |= BIT3;

    UCA0CTL0 &= ~UCSWRST; //Reactivem la línia de comunicacions sèrie
    // UCA1IE |= UCRXIE; //Això només s’ha d’activar quan tinguem la rutina de recepció
}

void TXUAC0(byte TXData)
{
    while(!TXD0_READY); //esperem la flag de la UART i esperem a que quedi buit el buffer de sortida
    UCA0TXBUF = TXData; //un cop buit el buffer enviem la dada carregant-la al buffer de sortida

}
