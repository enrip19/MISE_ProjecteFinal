/*
 *  miscLib.c
 *
 *  Created on: 9 may. 2020
 *      Author: Enric i sergio
 */
#include "miscLib.h"
#include "lcdLib.h"

uint8_t init_ucs_12MHz(){ //configuracio MCLK a 12MHz
    //Configuracion del reloj a 24MHz, a partir del DCO del micro.
    //Sin ningun ajuste, dejando todo por defecto, saldria MCLK del orden de 3MHz basado en el DCO.

    PJSEL0 |= BIT0 + BIT1; //activando pines de entradas LFXT de 32kHz

    ////Ajustes para utilizar el DCO a 12MHz:
    CS->KEY = 0x695A; //contrase�a para poder modificar los registros protegido CSCTLn
    CS->CTL0 = CS_CTL0_DCORSEL_4 ; // Seleccionamos rango de DCO 16MHz-32MHz
    //  sin ningun otro ajuste, se queda en la frec. nominal central de 24MHz.
    //posem el divisor a 2
    CS->CTL1 = CS_CTL1_SELS__DCOCLK |CS_CTL1_DIVS__2  |  CS_CTL1_SELM__DCOCLK; //Oscilador fuente para cada reloj

    //ACLK queda por defecto a LFXT, que es el cuarzo de 32kHz
    while (!(CS->STAT & (CS_STAT_MCLK_READY | CS_STAT_SMCLK_READY | CS_STAT_ACLK_READY))); //me espero hasta que el registro CSSTAT me indique que todos mis relojes se han estabilizado

      //  Activar los pines siguientes para comprobar y debugar los relojes -->
      P4SEL0 |= BIT2|BIT3|BIT4; //activo funciones alternativas primarias MCLK (P4.3), HSMCLK (P4.4) y ACLK (P4.2),
      //para poder observar los relojes con el osciloscopio.
      // <-- Volver a comentar una vez comprobado que los relojes estan bien configurados.
      //Los correspondientes bits en P4SEL1 han de estar a 0, que es su valor por defecto al iniciar el micro
      P4DIR |=BIT2|BIT3|BIT4;   //activo salida MCLK (P4.3), HSMCLK (P4.4) y ACLK (P4.2)
      // <-- Volver a comentar una vez comprobado que los relojes estan bien configurados.

     /*En este punto, tenemos:
      * MCLK = 24 MHz
      * SMCLK = 12 MHz
      * ACLK = LFXTCLK = 32 kHz (el cuarzo)
      */

    return 12;
}


void init_GPIO(void){
    /* Led RGB */
    P2->DIR |= BIT2 | BIT1 | BIT0;                                  //Inicialitzem els GPIO dels leds indicant que seran I/O
    P2->OUT =0;

    /* Led Vermell */
    P1->DIR |= BIT0;
    P1->OUT &= ~BIT0;

    /* Botons */
    P1->REN |= BIT4 | BIT1;                                         //Habilitem les R de pullup o pulldown
    P1->OUT |= BIT4 | BIT1;                                         //Seleccionem la R de pullup
    P1->IE |= BIT4 | BIT1;                                          //Habilitem interrupcions de P1
    P1->IFG = 0;                                                    //Fem un clear de totes les flags de P1

    buttonLFlag = 0;
    buttonRFlag = 0;
}

void init_NVIC(void){
    /* TimerA0 */
    NVIC->ICPR[0]|=BIT8;                                            //Borrem les posibles ISR's del timerA0 pel CCR0
    NVIC->ISER[0]|=BIT8;                                            //Habilitem les interrupcions del timerA0 pel CCR0

    /* Buttons */
    NVIC->ICPR[1]|=BIT3;                                            //Borrem les posibles ISR's dels botons (P1)
    NVIC->ISER[1]|=BIT3;                                            //Habilitem les interrupcions dels botons (P1)

    /* ADC14 */
    NVIC->ICPR[0]|= 1 << ADC14_IRQn;                                //Borrem les possibles ISR's de l'ADC14
    NVIC->ISER[0]|= 1 << ADC14_IRQn;                                //Habilitem les interrupcions de l'ADC14

    /* I2C */

    NVIC->ICPR[0] |= 1 << EUSCIB1_IRQn;                             //Borrem les possibles ISR's de la EUSCIB0
    NVIC->ISER[0] |= 1 << EUSCIB1_IRQn;                             //Habilitem les interrupcions de la EUSCIB0
}

void init_portMapping(void){
    PMAPKEYID = PMAPKEY;                                            //Desbloqueamos los registros de configuracion de Port Mapping
    P2MAP01 &= 0x00FF;                                              //Limpiamos Hi byte que corresponde a 2.1
    P2MAP01 |= PM_TA1CCR1A << 8;                                    //ver documentación, cap. 13
    P2SEL0 |= BIT1;                                                 //P2.1 como funcion alternativa
    P2DIR |= BIT1;                                                  // como salida (Led2_G)
}

/*ISR del port 1 (polsadors)*/
void PORT1_IRQHandler(void){
    uint8_t status = P1IV;                                          //Guardem el valor de IV del port 1 per a que faci clear de les flags d'interrupcio
    P1->IE = 0;                                                     //Deshabilitem les interrupcions de P1
    if (status == 0x04){                                            //Si la interrupcio prove del pin 1 (S1)
        //buttonLFlag = 1;                                            //Pujem la flag del boto S1 (esquerra = L)
        buttonStatus = 0;
    }
    if (status == 0xA){                                             //Si la interrupcio prove del pin 4 (S2)
        //buttonRFlag = 1;                                            //Pujem la flag del boto S4 (dreta = R)
        buttonStatus = 1;

    }
    P1->IE |= BIT4 | BIT1;                                          //Tornem a habilitar les interrupcions de P1
}
