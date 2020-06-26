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
#include "library/modesLib.h"

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

    init_ucs_12MHz();                                                       //inicialitzem el rellotge SMCLK a 12 MHz
    init_TA0();                                                             //inicialitzem TimerA0
    init_GPIO();                                                            //inicialitzem botons i leds
    init_ADC();                                                             //inicialitzem ADC14
    init_I2C();                                                             //inicialitzem I2C

    init_NVIC();                                                            //inicialitzem NVIC (interrupcions)
    __enable_interrupts();                                                  //habilitem les interrupcions


    //UART + Motors
    Init_UART();
    //oju! l'adrec,a forma part dels parametres i aqui lestem separant


    init_lcd();                                                             //inicialtizem la LCD
    welcomeMessage();

    /*//demo de moure cursor//
    robot_print(":P",6,1);                                                  //printem el missatge a la columna 6 fila de baix
    robot_print(":(",9,0);                                                  //printem el missatge a la columna 9 fila de dalt
    delay(5000);                                                            //esperem 5sec per poder veure el missatge
    */
    lcd_clear();                                                            //borrem tota la pantalla


    // PART DE LDR
    ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;                          // Start sampling/conversion

    SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;                                   // Wake up on exit from ISR
    __DSB();                                                                // Ensures SLEEPONEXIT takes effect immediately


    //inicialitzacio de variables dels menus
    cursor = 0;                                                             //variable que fa de cursor per apuntar al menu de modes
    enter = 0;                                                              //variable que selecciona el mode demanat
    modeSel = 0;                                                            //variable on es guarda el valor del mode seleccionat

    //PROGRAMA
    while (1)
    {
        //Menu de seleccio de mode/////////////////////////////////////////////////////////////////////////////
        while (enter == 0){                                                 //menu de seleccio de mode (fins que no es selecciona un mode no sortim del bucle)
            modeSel = modeMenu();                                           //funcio de seleccio de mode
        }
        //Mode seleccionat/////////////////////////////////////////////////////////////////////////////////////
        while (enter == 1){                                                 //quan hem seleccionat un mode
            if (buttonLFlag == 1) {                                         //mirem si volem sortirne
                enter = 0;                                                  //en cas afirmatiu retornem variable enter a 0 per anar al menu de seleccio de mode
                buttonLFlag = 0;                                            //baixem la flag del polsador
                break;                                                      //parem el bucle
            }
            else {
                //PWM + LCD////////////////////////////////////////////////////////////////////////////////////
                adcL = read_LDR(newADCL);                                   //llegim el valor al ldr esquerra
                motorWrite_LDR(0x01, adcL);                                 //escribim la dada al motor

                adcR = read_LDR(newADCR);                                   //llegim el valor de ldr dret
                motorWrite_LDR(0x02, adcR);                                 //escribim la dada al motor

                //Seleccio de modes////////////////////////////////////////////////////////////////////////////
                switch (modeSel) {                                          //segons el mode que triem
                    case 1:
                        autopilot(adcL, adcR);                              //mode de conduccio autonoma seguint la llum
                        break;
                    case 2:
                        buttonMode();                                       //mode de conduccio a partir de botons
                        break;
                    default:
                        robot_print("Waiting... LDR:            ",0,0);     //mode despera on es mostra els valors dels ldr (sensors)
                        break;
                }

                robot_print_LDR(adcR, adcL);                                //printem a la lcd els valors dels ldr (o adc)

                oldadcR = adcR;                                             //propaguem la senyal
                oldadcL = adcL;

                ADC14->IER0 |= ADC14_IER0_IE1;
                ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;              // Start a new sampling/conversion
            }
            P1->IE |= BIT4 | BIT1;                                          //Tornem a habilitar les interrupcions de P1
        }



        __sleep();                                                          //Posem el sistema a dormir (activem LPM0). En aquest cas ho hem configurat a l'inici del while
        __no_operation();                                                   // For debugger
    }
}
