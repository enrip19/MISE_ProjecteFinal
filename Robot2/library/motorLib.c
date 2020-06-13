/*
 * funcions.c
 *
 *  Created on: 23 abr. 2020
 *      Author: enric i sergio
 */
#include <msp432p401r.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "motorLib.h"
#include "uartLib.h"
#include "lcdLib.h"

//baix nivell
str_control arrayCreator(uint8_t id, uint8_t length, uint8_t instruccio, uint8_t address, uint8_t *p_array, uint8_t checkSum)       //funcio per crear larray que necessiten els motors
{
    str_control control;                                                            //creem una array del tipus str_control (struct amb una array interna de 20 posicions) on escriurem
    str_control *control_punter;                                                    //creem un punter del mateix tipus
    control_punter = &control;                                                      //fem apuntar el punter a la nostra array

    memset(control.array,'0', 20);                                                  //escribim tot a 0 en la nostra array

    //escribim les dades al lloc correcte mitjanc,ant el punter (lordre ve donat pel datasheet dels motors)
    control_punter -> array[0] = 0xFF;
    control_punter -> array[1] = 0xFF;
    control_punter -> array[2] = id;
    control_punter -> array[3] = length;
    control_punter -> array[4] = instruccio;
    control_punter -> array[5] = address;

    //per escriure els parametres, necessitem un punter mes petit (1 byte) que recorre els parametres un a un (p_array)
    uint8_t i;
    for (i=0; i<(length-3); i++)                                                    //fem un for amb la llargada dels parametres. length = sizeof(param) +3  per tant hem de restar per tenir la quantitat de parametres
    {
        control_punter -> array[i+6] = *p_array;                                    //anem escribint els parametres seguidament en la nostra array (o struct)
        p_array++;                                                                  //incrementem la posicio del punter de 1 byte per poder passar al seguent parametre
    }
    control_punter -> array[i+6] = checkSum;                                        //en acabar amb els parametres hem dafegir el checksum (que ens ve duna funcio externa)
    control_punter -> array[i+6+1] = 0x0A;                                          //posem un caracter de nova linia per despres ferlo servir de senyal per marcar quants bytes shan de enviar.
    return *control_punter;                                                         //finalment retornem el contingut de la nostra struct (ja que la funcio es del tipus daquesta)
    //return 0;
}

uint8_t checkSum_Calc(uint8_t id, uint8_t length, uint8_t instruccio, uint8_t address, uint8_t *p_array){                           //funcio per calcular automaticament el checksum
    uint8_t checkSum, i, param_array;                                               //creem les variables necessaries

    param_array=0;                                                                  //param_array sera el valor de la suma dels parametres. necessitem que incii a 0
    for(i=0; i<(length-3); i++){                                                    //length = sizeof(param) +3  per tant hem de restar per tenir la quantitat de parametres.
        param_array = param_array + *p_array;                                       //el que fem es anar sumant els parametres de forma recursiva. sumem la suma de parametres que duem mes el nou
        p_array++;                                                                  //incrementem la posicio del punter de 1 byte per poder passar al seguent parametre
    }
    checkSum = ~(id + length + instruccio + address + param_array);                 //fem el calcul del checksum a partir de la formula del datasheet dels motors. aqui ja tenim en compte en el length la adrec,a

    return checkSum;                                                                //finalment retornem el checksum
}


void send_Motor(uint8_t id, uint8_t length, uint8_t instruccio, uint8_t address, uint8_t *p_array){                                 //funcio per enviar instruccions als motors
    uint8_t checkSum;                                                               //creem les variables necessaries

    str_control Data;                                                               //creem una array del tipus str_control (struct amb una array interna de 20 posicions) on escriurem
    str_control *Data_point;                                                        //creem un punter del mateix tipus

    memset(Data.array,'0', 20);                                                     //escribim tot a 0 en la nostra array

    Data_point = &Data.array[0];                                                    //fem apuntar el punter a la nostra array
    checkSum = checkSum_Calc(id, length, instruccio, address, p_array);             //executem la funcio per calcular el checksum
    Data = arrayCreator(id, length, instruccio, address, p_array, checkSum);        //executem la funcio per crear la array per enviar la instruccio als motors
    send_UART(Data_point);                                                          //enviem la instruccio als motors mitjnac,ant UART
}

//alt nivell
void motorWrite_LDR(uint8_t id, uint8_t value){
    char param [] = {value};                                                  //escribim els parametres (adrec,a i valor en aquest cas)
    //uint8_t param_pointer;
    uint8_t instruccio = WRITE_MOT;                                                 //escribim a la instruccio el valor de WRITE (escriure a un registre del motor)
    uint8_t address = LDR_ADR;                                                      //escribim l'adrec,a del registre on guardem el valor de ldr
    uint8_t length = sizeof(param) + 3;                                             //instruccio + adreça + parametres + checksum

    send_Motor(id, length, instruccio, address, param);                             //finalment enviem la dada per UART al motor
}

void  motorGO (uint8_t id, float vel, bool direccion){

     vel=vel/100;                                                                   //Calculamos el valor del porcentaje
     uint16_t velocidad=(vel)*velmax;                                                //Obtenemos el valor equivalente de la velocidad, respecto del valor maximo
     uint8_t velhexL= velocidad;                                                    //Hacemos un cast de la variable velocidad, ya que esta ocupa dos registros del motor
     uint8_t velhexH=velocidad>>8;

     char param [] = {velhexL};
     uint8_t instruccio = WRITE_MOT;                                                 //escribim a la instruccio el valor de WRITE (escriure a un registre del motor)
     uint8_t address = MOVING_SPEED;                                                         //escribim l'adrec,a del registre on guardem el valor de ldr
     uint8_t length = sizeof(param) + 3;                                                 //instruccio + adreça + parametres + checksum
     send_Motor(id, length, instruccio, address, param);

    if(direccion){
              velhexH=velhexH^front;
                  char param [] = {velhexH};
                  uint8_t instruccio = WRITE_MOT;                                                 //escribim a la instruccio el valor de WRITE (escriure a un registre del motor)
                  uint8_t address = TURN_DIRECTION;                                                         //escribim l'adrec,a del registre on guardem el valor de ldr
                  uint8_t length = sizeof(param) + 3;                                                 //instruccio + adreça + parametres + checksum
                  send_Motor(id, length, instruccio, address, param);}
           else {
               velhexH=velhexH&back;
                   char param [] = {velhexH};
                   uint8_t instruccio = WRITE_MOT;                                                 //escribim a la instruccio el valor de WRITE (escriure a un registre del motor)
                   uint8_t address = TURN_DIRECTION;                                                         //escribim l'adrec,a del registre on guardem el valor de ldr
                   uint8_t length = sizeof(param) + 3;                                                 //instruccio + adreça + parametres + checksum
                   send_Motor(id, length, instruccio, address, param);}

     }

void robotGO (float velocity,bool direcction, float gir){

    //conversio de percentatge a 8bits

    uint8_t vel, velR, velL, deltaL, deltaR, girVar;
    vel = (uint8_t) velocity;
    //girVar = (uint8_t) gir;

    if (gir > 0){
        girVar = (uint8_t) gir;
        deltaL = 0;
        deltaR = girVar;
    }
    else if (gir < 0){
        girVar = (uint8_t) abs(gir);

        deltaR = 0;
        deltaL = girVar;
        //printf("girVar: %d, deltaL: %d\n",girVar, deltaL);
    }
    else{
        deltaR = 0;
        deltaL = 0;
    }


    velL = vel - deltaL;
    velR = vel - deltaR;
    printf("vel: %d, velL: %d, velR: %d\n",vel, velL, velR);




    if (direcction){
        motorGO (0x01,velL, 0);
        motorGO (0x02,velR, 1);
    }
    else{
        motorGO (0x01,velL, 1);
        motorGO (0x02,velR, 0);
    }
    robot_print_motor(velL,velR);
}

void autopilot (uint8_t left, uint8_t right ){
    uint8_t diff;
    uint8_t vel, velR, velL, deltaL, deltaR, girVar;
    float giro;
    diff =left-right;

    if (diff>0){

        giro=(left/255)*(-100);
        robotGO (left,1,giro);

        deltaL = 0;
        deltaR = giro;
        velL = vel - deltaL;
        velR = vel - deltaR;
    }
    else {
        giro=(right/255)*(-100);
        robotGO (right,1,giro);

        deltaR = 0;
        deltaL = giro;
    }

    velL = vel - deltaL;
    velR = vel - deltaR;

    motorGO (0x01,velL, 0);
    motorGO (0x02,velR, 1);
    robot_print_motor(velL,velR);

}
