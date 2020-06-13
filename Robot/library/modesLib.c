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

//Mode control per LDR (autopilot)/////////////////////////////////////////////////////////////////////
void autopilot (uint8_t left, uint8_t right ){
    float diff;
    float vel;
    float velR, velL;

    diff=left-right;
    if (abs(diff)<10){
        vel=left*(0.0039);
        vel=vel*100;
        motorGO (0x01,vel, 0);
        motorGO (0x02,vel, 1);
        robot_print_motor(vel,vel);}
    else if (diff>0){
        diff=(diff/255)*100;
        velL=left*(0.0039);
        velL=velL*100;
        velR=left-abs(diff);
        velR=velR/255;
        velR=velR*100;

        motorGO (0x01,velL, 0);
        motorGO (0x02,velR, 1);
        robot_print_motor(velL,velR);
    }
        else if (diff<0){
        diff=(diff/255)*100;
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
