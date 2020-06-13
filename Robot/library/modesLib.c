/*
 * modesLib.c
 *
 *  Created on: 12 jun. 2020
 *      Author: enric
 */

#include "modesLib.h"
#include "miscLib.h"
#include "motorLib.h"

//MODE DE CONTROL PER POLSADORS////////////////////////////////////////////////////////////////
void buttonMode(void){
        //robotGO(100,0, 50);
        switch (controlFlag) {
            case 1: //forward
                velButton+=10;
                break;
            case 2: //backward
                velButton-=10;
                break;
            case 3: //left
                if(velButton>=0){
                    girButton-=10;
                    if(girButton <= -velButton) girButton = -velButton;
                }
                else if(velButton<0){
                    girButton+=10;
                    if(girButton >= -velButton) girButton = -velButton;
                }
                break;
            case 4: //right
                if(velButton>=0){
                    girButton+=10;
                    if(girButton >= velButton) girButton = velButton;
                }
                else if(velButton<0){
                    girButton-=10;
                    if(girButton <= velButton) girButton = velButton;
                }
                break;
            default:
                velButton = velButton;
                girButton = girButton;
                sentitButton = sentitButton;
                break;
        }
        //printf("velButton: %d\n", velButton);
        if(velButton<=0) sentitButton = 0;
        else if(velButton>0) sentitButton = 1;

        if(velButton >= 100) velButton = 100;
        else if(velButton <= -100) velButton = -100;

        /*if(girButton >= velButton) girButton = velButton;
        else if(girButton <= -velButton) girButton = -velButton;*/


        controlFlag = 0;
        velButtonF = (float) abs(velButton);
        girButtonF = (float) girButton;
        //printf("velButtonF: %f, sentitButton: %d, girButtonF: %f\n",velButtonF,sentitButton,girButtonF);
        robotGO(velButtonF, sentitButton, girButtonF);

        P4->IE |= BIT1 | BIT2 | BIT3 | BIT4;
}
        ///////////////////////////////////////////////////////////////////////////////////////////////
