/*
 * uart.c
 *
 *  Created on: 6 jun. 2020
 *      Author: enric
 */
#include "uartLib.h"
#include "timerLib.h"



void Init_UART(void)
{
    UCA0CTL0 |= UCSWRST;                                    //Fem un reset de la USCI i que estigui inoperativa
    UCA0CTL1 = 0x00;                                        //UCSYNC=0 mode asíncron
                                                            //UCMODEx=0 seleccionem mode UART
                                                            //UCSPB=0 nomes 1 stop bit
                                                            //UC7BIT=0 8 bits de dades
                                                            //UCMSB=0 bit de menys pes primer
                                                            //UCPAR=x no es fa servir bit de paritat
                                                            //UCPEN=0 sense bit de paritat
    UCA0CTL0 |= UCSSEL__SMCLK;                              //Triem SMCLK (16MHz) com a font del clock BRCLK

    //Baud rate config (115200)
    UCA0BRW = 6;
    ucos16 = 1;
    ucbrf = 8;                                              // posem valor a ucbrf
    ucbrf <<= 4;                                            //el desplacem 4 bits per posar-lo al lloc que toca
    ucbrs = 0x20;
    ucbrs <<= 8;

    UCA0MCTLW |= (ucos16 | ucbrf | ucbrs);

    P1SEL0 |= (BIT2 | BIT3);

    P1SEL1 = 0;                                             //I/O funció: P1.2 = UARTRX, P1.3 = UARTTX
    P1REN |= BIT2;                                          //amb resistència activada de pull-up l’entrada P4.52
    P1OUT |= BIT2;
    P1DIR |= BIT3;

    UCA0CTL0 &= ~UCSWRST;                                   //Reactivem la línia de comunicacions sèrie
    UCA1IE |= UCRXIE;                                       //Aixo nomes sha dactivar quan tinguem la rutina de recepcio
}

void TXUAC0(byte TXData)
{
    while(!TXD0_READY);                                     //esperem la flag de la UART i esperem a que quedi buit el buffer de sortida
    UCA0TXBUF = TXData;                                     //un cop buit el buffer enviem la dada carregant-la al buffer de sortida

}

void send_UART(str_control *dataPointer){
    int indexUART;                                          //creem un index pel for de la UART
    uint8_t dada, lengthArray;                              //creem una variable de 1 byte per allotjar la dada que enviem per uart. Tambe una per mirar la mida de larray sencera

    lengthArray = (sizeof(dataPointer->array));             //calculem la llargada de larray

    for(indexUART=0;indexUART<lengthArray;indexUART++){     //el for servira per enviar tota la array byte a byte per uart
        dada = dataPointer->array[indexUART];               //lectura de la dada que toca (ho sabem amb lindex) i la guardem a la variable denviament
        if(dada == 0x0A){                                   //en cas de trobar-nos amb un \n (o 0x0A) no escribim mes
            break;                                          //aturem el for
        }
        else{                                               //en cas contrari:
            TXUAC0(dada);                                   //enviem la dada per uart
        }
    }
    delay(40);                                             //quan acabem esperem 100 ms ja que es el que ens demanen els motors (o emulador) entre trames (hem forc,at la maquina per ferlo funcionar mes rapid)
}

/*
//ISR RX
void EUSCIA0_IRQHandler(void){
    EUSCI_A0 -> IE &= ~BIT0;                                 //desactivem interrupcions de Rx
    RXData = EUSCI_A0 -> RXBUF;                             //llegim la dada que arriba al buffer de Rx
    RXFlag = 1;                                             //activem la flag de recepcio de la UART
    EUSCI_A0 -> IE |= BIT0;                                  //reactivem interrupcions de Rx
}*/
