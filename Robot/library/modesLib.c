/*
 * modesLib.c
 *
 *  Created on: 12 jun. 2020
 *      Author: enric
 */

#include "modesLib.h"
#include "miscLib.h"
#include "motorLib.h"
#include "lcdLib.h"

//MODE DE CONTROL PER POLSADORS////////////////////////////////////////////////////////////////
void buttonMode(void){
        //robotGO(100,0, 50);
        switch (controlFlag) {                                                          //Depenent dels 4 botons de control pitjats (forward, backward, left i right)
            case 1: //forward
                velButton+=10;                                                          //augmentem la velocitat 10 punts
                break;
            case 2: //backward
                velButton-=10;                                                          //disminuim la velocitat 10 punts
                break;
            case 3: //left
                if(velButton>=0){                                                       //si la velocitat es positiva
                    girButton-=10;                                                      //el gir el disminuim 10 punts
                    if(girButton <= -velButton) girButton = -velButton;                 //aixo ho fem per que funcioni igual anant endavant que enderrera
                }
                else if(velButton<0){                                                   //si la velocitat es negativa (anem endarrera)
                    girButton+=10;                                                      //fem el mateix pero al reves de quan anem endavant
                    if(girButton >= -velButton) girButton = -velButton;
                }
                break;
            case 4: //right
                if(velButton>=0){                                                       //Hem de fer el mateix que quan anem cap a lesquerra pero tot a linversa
                    girButton+=10;
                    if(girButton >= velButton) girButton = velButton;
                }
                else if(velButton<0){
                    girButton-=10;
                    if(girButton <= velButton) girButton = velButton;
                }
                break;
            default:
                velButton = velButton;                                                  //en cas de no pitjar cap boto mantenim valors de totes les variables
                girButton = girButton;
                sentitButton = sentitButton;
                break;
        }
        //printf("velButton: %d\n", velButton);
        if(velButton<=0) sentitButton = 0;                                              //si la velocitat es negativa posem el sentit a 0 (es a dir, anem endarrere)
        else if(velButton>0) sentitButton = 1;                                          //si la velocitta es positiva posem el sentit a 1 (es a dir, anem endavant)

        if(velButton >= 100) velButton = 100;                                           //si la velocitat supera el limit de 100 o -100 (%) no permetem passar daquests valors
        else if(velButton <= -100) velButton = -100;

        /*if(girButton >= velButton) girButton = velButton;
        else if(girButton <= -velButton) girButton = -velButton;*/


        controlFlag = 0;                                                                //tornem a 0 el controlFlag (es a dir, que no queda cap polsador pitjat)
        velButtonF = (float) abs(velButton);                                            //agafem el valor absolut de la velocitat i ho convertim a float per entrarho a la funcio robotGo (ja que ho demana aixi)
        girButtonF = (float) girButton;                                                 //agafem el valor del gir i ho convertim a float per entrarho a la funcio robotGo (ja que ho demana aixi)
        //printf("velButtonF: %f, sentitButton: %d, girButtonF: %f\n",velButtonF,sentitButton,girButtonF);
        robotGO(velButtonF, sentitButton, girButtonF);                                  //movem el robot amb la velocitat, sentit i gir marcats anteriorment

        P4->IE |= BIT1 | BIT2 | BIT3 | BIT4;                                            //habilitem interrupcions dels botons de control
}
        ///////////////////////////////////////////////////////////////////////////////////////////////

//Mode control per LDR (autopilot)/////////////////////////////////////////////////////////////////////
void autopilot (uint8_t left, uint8_t right ){
    float diff;
    float vel;
    float velR, velL;

    diff=left-right;                                                                    //agafem la diferencia de valors dels ldr per eliminar el factor ambiental

    if (abs(diff)<(sensibilidad)){                                                      //si el valor absolut de la diferencia es mes petit que la sensibilitat. per tant, hem danar recte
        vel=left*(0.0039);                                                              //convertim el valor del ldr en un percentatge respecte el 100% de la velocitat. fem servir la
        vel=vel*100;                                                                    //velocitat del ldr esquerre de forma arbitraria
        motorGO (0x01,vel, 0);                                                          //posem la velocitat calculada als motors per a que vagi recte
        motorGO (0x02,vel, 1);
        robot_print_motor(vel,vel);}                                                    //printem les velocitats a la lcd
    else if (diff>0){                                                                   //en cas que la diferencia sigui positiva, vol dir que tenim mes llum a lesquerra que a la dreta. Hem de fer un gir a lesquerra
        diff=(diff/255)*100;                                                            //fem el mateix, calculem un percentatge respecte el 100% de la velocitat, aconsguint una vel inferior, pero ara pel motor esquerra
        velL=left*(0.0039);                                                             //daquesta manera aconseguim fer un gir
        velL=velL*100;
        velR=left-abs(diff);                                                            //un cop calculat el valorde la velocitat de lesquerra, calculem el dret fent la resta amb la diferencia
        velR=velR/255;                                                                  //i ho convertim en un percentatge altre cop
        velR=velR*100;

        motorGO (0x01,velL, 0);                                                         //Escribim al motor esquerra la velocitat calculada per aquest
        motorGO (0x02,velR, 1);                                                         //escribim al motor dret la velocitat calculada per aquest
        robot_print_motor(velL,velR);                                                   //printem les velocitats a la lcd
    }
        else if (diff<0){                                                               //si la diferencia es negativa vol dir que tenim mes llum a la dreta, i per tant, hem de fer un gir a la dreta
        diff=(diff/255)*100;                                                            //seguim el mateix procediment que abans pero amb els motors canviats
        velR=right*(0.0039);
        velR=velR*100;
        velL=right-abs(diff);
        velL=velL/255;
        velL=velL*100;
        motorGO (0x01,velL, 0);
        motorGO (0x02,velR, 1);
        robot_print_motor(velL,velR);
        }
    ;
}
        /////////////////////////////////////////////////////////////////////////////////////////////////
