/*
 * i2cLib.c
 *
 *  Created on: 22 may. 2020
 *      Author: enric i sergio
 */
#include "i2cLib.h"

void init_I2C (void){
    /* Pins Config */
    //Configuracio dels pins 6.4 i 6.5 per fer la funcio secundaria de I2C
    P6 -> SEL0 |= BIT4 + BIT5;
    P6 -> SEL1 |= 0;
    /* I2C config */
    UCB1CTLW0 |= UCSWRST;
    UCB1CTLW0 |= UCMST|UCMODE_3|UCSYNC|UCSSEL_2|UCSWRST;                 //MODO MASTER , USCI MODO I2C , SMCLK
    UCB1BR0=120;
    UCB1BR1=0 ;

    UCB1CTLW0 &= ~UCSWRST;
    EUSCI_B1->IE |= EUSCI_B_IE_TXIE0 |                                  // Enable transmit interrupt
                EUSCI_B_IE_NACKIE;                                      // Enable NACK interrupt
}


void I2C_send (uint8_t addr, uint8_t *buffer, uint8_t n_dades ){
    UCB1I2CSA = addr;                                                   //Coloquem l�adre�a de slave
    PTxData = buffer;                                                   //adrec,a del bloc de dades a transmetre
    TXByteCtr = n_dades;                                                //carreguem el n�mero de dades a transmetre;
    while(EUSCI_B1 ->CTLW0 & EUSCI_B_CTLW0_TXSTP);                      //Ens assegurem que s'ha enviat la condició de stop
    UCB1CTLW0 |= UCTXSTT | UCTR;                                        //condicion START, escritura
    /* POSEM A DORMIR EL SISTEMA EN MODE EQUIVALENT A LPM0 + IE */
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;                                //Configurem el mode LPM0 (CPU apagada, nomes funcionen interrupcions)
    __DSB();                                                            //Acabem totes les transferencies (en aquest cas no en tenim)
    __sleep();                                                          //Posem el sistema a dormir (activem LPM0)

    __no_operation();                                                   //Resta en mode LPM0 fins que es trasmetin les dades
    while(EUSCI_B1 -> CTLW0 & UCTXSTP);                                 //Ens assegurem que s'ha enviat la condició de stop
}

void I2C_receive(uint8_t addr, uint8_t *buffer, uint8_t n_dades)
{
    PRxData = buffer;                                                   //adreça del buffer on ficarem les dades rebudes
    RXByteCtr = n_dades;                                                //carreguem el número de dades a rebre
    UCB1I2CSA = addr;                                                   //Coloquem l’adreça de slave
    UCB1CTL1 &= ~UCTR;                                                  //I2C en mode Recepció
    while (UCB1CTL1 & UCTXSTP);                                         //Ens assegurem que el bus està en stop
    UCB1CTL1 |= UCTXSTT;                                                //I2C start condition en recepció
    /* POSEM A DORMIR EL SISTEMA EN MODE EQUIVALENT A LPM0 + IE */
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;                                //Configurem el mode LPM0 (CPU apagada, nomes funcionen interrupcions)
    __DSB();                                                            //Acabem totes les transferencies (en aquest cas no en tenim)
    __sleep();                                                          //Posem el sistema a dormir (activem LPM0)

}

//IRQ I2C
void EUSCIB1_IRQHandler(void){//
    uint16_t status = EUSCI_B1->IV;                                         //guardem el valor de IV a la variable status. Aixo cleareja totes les flags, en consultar-lo
    switch(status){
    case 0x16:
        if(RXByteCtr){
          *PRxData++ = EUSCI_B1 -> RXBUF;                                   //guardem la dada al valor seguent on apunta PRxData
          if(RXByteCtr == 1){                                               //Queda només 1 dada?
               EUSCI_B1 -> CTLW0 |= UCTXSTP;                                //Llavors enviem un stop byte
          }
          else{                                                             //en cas contrari
             *PRxData = EUSCI_B1 -> RXBUF;                                  //guardem la dada rebuda a l'adrc,a de PRxData
          }
        }
          break;
    case 0x18:
        if(TXByteCtr){
          EUSCI_B1 -> TXBUF = *PTxData++;                                   //Carreguem a TXBUFF la dada a enviar
          TXByteCtr--;
          }
        else{
         EUSCI_B1 -> CTLW0 |= UCTXSTP;                                   //I2C stop condition
         /* PREGUNTA: PERQUE S'HA DE POSAR AIXO? */
         // Clear USCI_B1 TX int flag
         EUSCI_B1->IFG &= ~EUSCI_B_IFG_TXIFG;
         // Wake up on exit from ISR
         SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
         // Ensures SLEEPONEXIT takes effect immediately
         __DSB();
         /* FI DE LA PREGUNTA*/
        }
        break;
    default: break;
    }
}
