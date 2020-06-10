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

    init_ucs_12MHz();                                                       //inicialitzem el rellotge SMCLK a 12 MHz
    init_TA0();                                                             //inicialitzem TimerA0
    init_TA1(pwmMax, pwm);                                                  //inicialitzem el TimerA1
    init_GPIO();                                                            //inicialitzem botons i leds
    init_ADC();                                                             //inicialitzem ADC14
    init_portMapping();                                                     //inicilaitzem el portmapping del led verd (P2.1) amb el PWM
    init_I2C();                                                             //inicialitzem I2C

    init_NVIC();                                                            //inicialitzem NVIC (interrupcions)
    __enable_interrupts();                                                  //habilitem les interrupcions


    //UART + Motors
    Init_UART();
    //oju! l'adrec,a forma part dels parametres i aqui lestem separant

    //LCD
    //maxims i minims del pwm
    sensibilitat = 8;                                                       //sensibilitat de la iluminacio
    pwmMax = 256;                                                           //maxim valor de inici de  ccr0
    pwm = 128;                                                              //valor de inici de ccr1

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
    robot_print("Valor ADC (PWM):",0,0);                                    //printem el missatge a la columna 0 fila de baix

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

        //PWM + LCD
        motorGO (0x01,50,0);
        delay(20);                                                          // Delay
        if(buttonStatus ==1){                                               //si apretem el bot� S2 (posem el buttonStatus a 1) i engeguem el sistema
            if(adcFlag==1){                                                 //si la interrupcio de ADC esta activa (per tant s'aixeca la flag)
                    //printf("%u,%u,%u\n", newADC,oldADC,pwm);              //printem algunes variables per debuggejar
                    if(pwm<=10){                                            //Si la var pwm es inferior o igual a 10
                            pwm = 10;                                       //forcem la variable a 10 per a no tenir overflow
                            P1->OUT |= BIT0;                                //Engeguem el led vermell (Led1) per marcar que hem arribat al minim de llum del led RGB
                        if((newADC-oldADC)<=(-sensibilitat)){               //Restem els dos valors l'antic i el nou del valor llegit per l'adc i el comparem amb una sensibilitat. En cas de ser correcta...
                            pwm+=10;                                        //Augmentem el valor del comptador ccr1, i per tant, disminuim el pwm
                            }
                        }
                    else if(pwm>=pwmMax){                                   //Si la var pwm es superior o igual a pwmMax (valor ccr0)
                            pwm = pwmMax;                                   //forcem la variable pwm a pwmMax (valor ccr0) per no tenir overflow.
                            P1->OUT |= BIT0;                                //Engeguem el led vermell (Led1) per marcar que hem arribat al maxim de llum del led RGB
                            if((newADC-oldADC)>=sensibilitat){              //Restem els dos valors l'antic i el nou del valor llegit per l'adc i el comparem amb una sensibilitat. En cas de ser correcta...
                                pwm-=10;                                    //Disminuim el valor del comptador ccr1, i per tant, augmentem el pwm
                            }


                        }
                    else {                                                  //En cas de estar en la regio de valors on no son un maxim
                            P1->OUT &= ~BIT0;                               //Desactivem el led vermell (Led1)
                            if((newADC-oldADC)>=sensibilitat){              //Restem els dos valors l'antic i el nou del valor llegit per l'adc i el comparem amb una sensibilitat. En cas de ser correcta...
                                pwm-=10;                                    //Disminuim el valor del comptador ccr1, i per tant, augmentem el pwm

                            }
                            else if((newADC-oldADC)<=(-sensibilitat)){      //Restem els dos valors l'antic i el nou del valor llegit per l'adc i el comparem amb una sensibilitat. En cas de ser correcta...
                                pwm+=10;                                    //Augmentem el valor del comptador ccr1, i per tant, disminuim el pwm
                            }
                        }

                    oldADC = newADC;                                        //guardem el valor actual a la variable de valor antic per poder comparar en la seguent iteracio amb el nou valor llegit per l'ADC
                    TA1CCR1=pwm;                                            //Un cop calculat el valor de pwm podem posarlo al contador ccr1
                    adcFlag = 0;                                            //Abaixem la flag de l'accelerometre
                    ADC14->IER0 |= ADC14_IER0_IE0;                          // Enable ADC conv complete interrupt
                }

                ADC14->CTL0 |= ADC14_CTL0_ENC | ADC14_CTL0_SC;              // Start a new sampling/conversion
                robot_print_var(newADC, pwm, 0,1);                          //printem el valor de l'ADC i el PWM a la columna 0 i linia 1
                motorWrite_LDR(0x01, pwm);

        }
            else{                                                           //en cas d'apretar el boto de l'esquerra S1 (per apagar el led)
                pwm=pwmMax+10;                                              //posem un valor de CCR1 major al de CCR0 fent que el valor de PWM sigui 0 de forma constant
                TA1CCR1=pwm;                                                //Apliquem el valor de CCR1 al seu registre
                robot_print("Push S2 button",0,1);                          //printem un missatge d'espera a la columna 0 i linia 1
            }

        __sleep();                                                          //Posem el sistema a dormir (activem LPM0). En aquest cas ho hem configurat a l'inici del while
        __no_operation();                                                   // For debugger
    }
}
