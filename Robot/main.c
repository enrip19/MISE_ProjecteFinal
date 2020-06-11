/*
 * Program created by: Sergio Diaz i Enric Puigvert
 * Universitat de Barcelona
 * Microcontroladors i Sistemes Empotrats
 */

#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/* includes de les llibreries*/

#include "library/miscLib.h"
#include "library/timerLib.h"
#include "library/adcLib.h"
#include "library/i2cLib.h"
#include "library/lcdLib.h"
#include "library/motorLib.h"
#include "library/uartLib.h"

//uint8_t buffer[2];


const uint8_t arrayDades [] = {  //Array dades
    0xFF, 0xFF, 0X00, 0X04, 0x02, 0X03, 0x01, 0xF5
};






/*Daescripcio:
*/


 int main(void) {

    WDT_A->CTL = WDT_A_CTL_PW |                                             // Stop WDT
                 WDT_A_CTL_HOLD;
    __disable_interrupt();

    str_recursive recursive;
    str_recursive *p_recursive;
    p_recursive = &recursive;

    //LCD
    //maxims i minims del pwm
    uint8_t sensibilitat = 8;                                                       //sensibilitat de la iluminacio
    uint16_t motVelMax = 256;                                                           //maxim valor de inici de  ccr0
    p_recursive -> motVel = 128;                                                             //valor de inici de ccr1
    uint16_t motVel = 128;
    //p_recursive -> diff= 0;

    init_ucs_12MHz();                                                       //inicialitzem el rellotge SMCLK a 12 MHz
    init_TA0();                                                             //inicialitzem TimerA0
    init_TA1(motVelMax,motVel);                                                  //inicialitzem el TimerA1
    init_GPIO();                                                            //inicialitzem botons i leds
    init_ADC();                                                             //inicialitzem ADC14
    init_portMapping();                                                     //inicilaitzem el portmapping del led verd (P2.1) amb el PWM
    init_I2C();                                                             //inicialitzem I2C

    init_NVIC();                                                            //inicialitzem NVIC (interrupcions)
    __enable_interrupts();                                                  //habilitem les interrupcions


    //UART + Motors
    Init_UART();
    //oju! l'adrec,a forma part dels parametres i aqui lestem separant



    init_lcd();                                                             //inicialtizem la LCD
    robot_print("Hellowda!!!",2,0);                                         //printem el missatge a la columna 2 fila de dalt
    delay(2000);                                                            //esperem 2sec per poder veure el missatge
    robot_print("Iniciant Sistema...", 0,1);                                //printem el missatge a la columna 0 fila de baix
    delay(2000);                                                            //esperem 2sec per poder veure el missatge
    lcd_clear();                                                            //borrem tota la pantalla
    delay(10);                                                              //esperem a que processi la lcd

    //demo de moure cursor//
    robot_print(":P",6,1);                                                  //printem el missatge a la columna 6 fila de baix
    robot_print(":(",9,0);                                                  //printem el missatge a la columna 9 fila de dalt
    delay(5000);                                                            //esperem 5sec per poder veure el missatge

    lcd_clear();                                                            //borrem tota la pantalla
    robot_print("Valor ADC L i R:",0,0);                                    //printem el missatge a la columna 0 fila de baix

    // PART DE LDR
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;                          // Start sampling/conversion

    SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;                                   // Wake up on exit from ISR
    __DSB();                                                                // Ensures SLEEPONEXIT takes effect immediately



    //PROGRAMA
    while (1)
    {
        //UART
        //send_Motor(id, length, instruccio, address, p_array);                           //funcio per enviar instruccions als motors

        ///(DEBUGGING)
        /*int j;
        uint8_t dada;
        for(j=0; j<sizeof(arrayDades);j++){
            dada = arrayDades[j];
            TXUAC0(dada);
        }*///
        /*adcFlag = 0;                                            //Abaixem la flag de l'accelerometre
        ADC14->IER0 |= ADC14_IER0_IE1;                          // Enable ADC conv complete interrupt
    }

    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;              // Start a new sampling/conversion
    printf("%d; %d \n",diff, motVel);
    robot_print_var(newADCL, newADCR, 0,1);                          //printem el valor de l'ADC i el motVel a la columna 0 i linia 1
    motorWrite_LDR(0x01, newADCL);
    motorWrite_LDR(0x02, newADCR);*/


        //PWM + LCD
        robotGO (40,0);
        delay(20);                                                          // Delay
        if(buttonStatus ==1){                                               //si apretem el bot� S2 (posem el buttonStatus a 1) i engeguem el sistema
            adcR = read_LDR(newADCR);                                       //llegim el valor de ldr dret
            adcR = (adcR+oldadcR)>>1;                                       //fem una mitjana entre el valor actual i lanterior per estabilitzar la senyal
            adcL = read_LDR(newADCL);
            adcL = (adcL+oldadcL)>>1;

            robot_print_var(adcL,adcR, 0,1);
            motorWrite_LDR(0x01, adcR);
            motorWrite_LDR(0x02, adcL);

            oldadcR = adcR;                                                 //propaguem la senyal
            oldadcL = adcL;


        }
        else{                                                           //en cas d'apretar el boto de l'esquerra S1 (per apagar el led)
            robot_print("Push S2 button",0,1);                          //printem un missatge d'espera a la columna 0 i linia 1
        }
        ADC14->IER0 |= ADC14_IER0_IE1;
        ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;              // Start a new sampling/conversion


        __sleep();                                                          //Posem el sistema a dormir (activem LPM0). En aquest cas ho hem configurat a l'inici del while
        __no_operation();                                                   // For debugger
    }
}
