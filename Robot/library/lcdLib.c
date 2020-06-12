/*
 * lcdLib.c
 *
 *  Created on: 22 may. 2020
 *      Author: enric i sergio
 */

#include "lcdLib.h"
#include "timerLib.h"
#include "i2cLib.h"
#include "modesLib.h"

void lcd_send_nibble_cmd(uint8_t dada){                                 //funcion para enviar las direccion del esclavo
    uint8_t buffer[2];                                                  //definimos el tamaï¿½o del buffer
    uint8_t dada_I2C_H;                                                 //dato a enviar en este caso sera el de inicializacion
    dada_I2C_H = dada & 0xF0;                                           //nuestra variable es de 4 bit por lo tanto operamos con and el valor 11110000 y de esta manera tendremos nuestro dato de 4 bits en un byte
                                                                        //Rellenamos los valores del buffer con las instrucciones de la lcd
    buffer[0] = dada_I2C_H | BACKLIGHT_LCD| E_LCD | CMD_LCD;            //primero introducimos nuestro dato, despues introducimos la instruccion de activar el display, despues instruccion activar el cursor,
                                                                        //por ultimo apagamos la pantalla, el parpadeo del cursor y el cursor.
    buffer[1] = dada_I2C_H | BACKLIGHT_LCD| CMD_LCD;                    //Primero se introduce el dato, despues se introduce la instruccion de activar la pantalla y se apaga la pantalla el cursor y el parpadeo
    I2C_send(LCD_ADDRESS, buffer, 2);                                   //Enviamos la direccion del esclavo, el buffer y la longitud del mismo a la funcion que nos lo enviara a la pantalla
}

void lcd_send_byte_cmd(uint8_t dada){
    uint8_t buffer[4];                                                  //creem una array de 4 bytes
    uint8_t dada_I2C_L, dada_I2C_H;                                     //creem dos variables per poder dividir la dada en 2
    dada_I2C_H = dada & 0xF0;                                           //guardem la part alta de la dada
    dada_I2C_L = (dada << 4) & 0xF0;                                    //guardem la part baixa de la dada
    //sempre enviem una part de la dada de 4bits (igual que amb send_nible) i li afegim l'habilitacio de la lcd, el control del led actiu, i que es una comanda
    buffer[0] = dada_I2C_H | BACKLIGHT_LCD| E_LCD | CMD_LCD;            //posem a la primera dada a enviar la part alta de la dada i habilitem la lcd
    buffer[1] = dada_I2C_H | BACKLIGHT_LCD| CMD_LCD;                    //mantenim la mateixa dada a enviar pero deshabilitant la lcd
    buffer[2] = dada_I2C_L | BACKLIGHT_LCD| E_LCD | CMD_LCD;            //fem el mateix per la part baixa
    buffer[3] = dada_I2C_L | BACKLIGHT_LCD| CMD_LCD;                    //mantenim la mateixa dada a enviar pero deshabilitant la lcd
    I2C_send(LCD_ADDRESS, buffer, 4);                                   //finalment, enviem l'array de dades mitjanc,ant el bucle que tenim determinat a la interrupcio
}

void lcd_send_byte_dada(uint8_t dada){
    uint8_t buffer[4];                                                  //creem una array de 4 bytes
    uint8_t dada_I2C_L, dada_I2C_H;                                     //creem dos variables per poder dividir la dada en 2
    dada_I2C_H = dada & 0xF0;                                           //guardem la part alta de la dada
    dada_I2C_L = (dada << 4) & 0xF0;                                    //guardem la part baixa de la dada
    //sempre enviem una part de la dada de 4bits (igual que amb send_nible) i li afegim l'habilitacio de la lcd, el control del led actiu, i que es una dada
    buffer[0] = dada_I2C_H | BACKLIGHT_LCD | E_LCD | DADA_LCD;          //posem a la primera dada a enviar la part alta de la dada i habilitem la lcd
    buffer[1] = dada_I2C_H | BACKLIGHT_LCD | DADA_LCD;                  //mantenim la mateixa dada a enviar pero deshabilitant la lcd
    buffer[2] = dada_I2C_L | BACKLIGHT_LCD | E_LCD | DADA_LCD;          //fem el mateix per la part baixa
    buffer[3] = dada_I2C_L | BACKLIGHT_LCD | DADA_LCD;                  //mantenim la mateixa dada a enviar pero deshabilitant la lcd
    I2C_send(LCD_ADDRESS, buffer, 4);                                   //finalment, enviem l'array de dades mitjanc,ant el bucle que tenim determinat a la interrupcio
}

void init_lcd(void){                                                    //cada vegada que enviem un byte/nibble, hem de tirar un delay per a esperar a que processi la pantalla
    delay(15);                                                          //esperem a que processi la lcd
    lcd_send_nibble_cmd(INIT_LCD);                                      //Byte d'inicialitzacio de la LCD
    delay(5);                                                           //esperem a que processi la lcd
    lcd_send_nibble_cmd(INIT_LCD);                                      //Byte d'inicialitzacio de la LCD
    delay(1);                                                           //esperem a que processi la lcd
    lcd_send_nibble_cmd(INIT_LCD);                                      //Byte d'inicialitzacio de la LCD
    delay(5);                                                           //esperem a que processi la lcd
    lcd_send_nibble_cmd(BIT4_COMM);                                     //Set LCD to 4 bit interface
    delay(1);                                                           //esperem a que processi la lcd
    lcd_send_byte_cmd(LINE2_5x8);                                       //2 lines, Font 5x8
    delay(1);                                                           //esperem a que processi la lcd
    lcd_send_byte_cmd(LCD_OFF);                                         //Display off, Cursor off, Blink off
    delay(1);                                                           //esperem a que processi la lcd
    lcd_send_byte_cmd(LCD_CLEAR);                                       //Clear Screen, Cursor Home
    delay(10);                                                          //esperem a que processi la lcd
    lcd_send_byte_cmd(INCR_MODE);                                       //Incrementmode...
    delay(10);                                                          //esperem a que processi la lcd
    lcd_send_byte_cmd(LCD_ON);                                          //Display ON, cursor OFF, cursor Blink
    delay(10);                                                          //esperem a que processi la lcd
}

void lcd_print(uint8_t *string, uint8_t mida){
    uint8_t i;
    for (i=0; i<mida; i++){                                             //enviarem tants bytes com sigui la mida de la nostra string
        lcd_send_byte_dada(string[i]);                                  //enviem el byte un a un
        delay(2);                                                       //ens esperem per que la lcd pugui processar
    }
    delay(2);                                                           //esperem a que processi la lcd
}

void lcd_clear(void){
    lcd_send_byte_cmd(LCD_CLEAR);                                       //Limpia la pantalla y resetea el cursor
    delay(10);                                                          //esperem a que processi la lcd
}

void lcd_setCursor(uint8_t col, uint8_t row){
    uint8_t i;
    lcd_send_byte_cmd(RTN_TO_HOME);                                     //resetea el cursor a la primera fila i primera columna
    delay(10);                                                          //esperem a que processi la lcd
    if (row == 0){                                                      //si la fila es 0 ens mantenim a la primera
        for (i=0; i<col; i++){                                          //repetirem el moviment tantes vegades com es demani a col
            lcd_send_byte_cmd(GO_RIGHT);                                //desplaza una posicion a la derecha el cursor
            delay(10);                                                  //esperem a que processi la lcd
        }
    }
    else if (row == 1){                                                 //si la fila es 1
        lcd_send_byte_cmd(DOWN_LINE);                                   //Baja de fila
        delay(10);                                                      //esperem a que processi la lcd
        for (i=0;i<col;i++){                                            //repetirem el moviment tantes vegades com es demani a col
            lcd_send_byte_cmd(GO_RIGHT);                                //Desplaza una posicion a la derecha el cursor
            delay(10);                                                  //esperem a que processi la lcd
        }
    }
    else{
        lcd_clear();                                                    //Limpia la pantalla y resetea el cursor
        delay(10);                                                      //esperem a que processi la lcd
        lcd_send_byte_cmd(RTN_TO_HOME);                                 //resetea el cursor a la primera fila i primera columna
        delay(10);                                                      //esperem a que processi la lcd
        mida_txt = sprintf(text, "ERROR: NO COLUMN");                   //mida_text reb la mida de la array que conte la string que guardem a text.
        lcd_print(text,mida_txt);                                       //printem la variable text
        delay(10);                                                      //esperem a que processi la lcd
        lcd_send_byte_cmd(DOWN_LINE);                                   //resetea el cursor a la primera fila i segona columna
        delay(10);                                                      //esperem a que processi la lcd
        mida_txt = sprintf(text, "SET ANOTHER COL.");                   //mida_text reb la mida de la array que conte la string que guardem a text.
        lcd_print(text,mida_txt);                                       //printem la variable text
        delay(10);                                                      //esperem a que processi la lcd
    }
}

void robot_print(char * string, uint8_t col, uint8_t row){              //aquesta sera la macro-funcio que l'usuari de la llibreria fara servir per printar text
    uint8_t mida_txt;
    char text[50];
    lcd_setCursor(col,row);                                             //posem el cursor on es demani a col i row
    delay(10);                                                          //esperem a que processi la lcd
    mida_txt = sprintf(text, string);                                   //mida_text reb la mida de la array que conte la string que guardem a text.
    lcd_print(text, mida_txt);                                          //printem la variable text
    delay(10);                                                          //esperem a que processi la lcd
}
void robot_print_var(uint8_t var0, uint8_t var1, uint8_t col, uint8_t row){ //aquesta sera la macro-funcio que l'usuari de la llibreria fara servir per printar variables
    uint8_t mida_txt;
    char text[50];
    lcd_setCursor(col,row);
    delay(10);                                                          //esperem a que processi la lcd
    mida_txt = sprintf(text, "%d - (%d)    ", var0, var1);              //mida_text reb la mida de la array que conte la string que guardem a text. La string sera en decimal la "var0 - var1"
    lcd_print(text, mida_txt);                                          //printem la variable text
    delay(10);                                                          //esperem a que processi la lcd
}



void robot_print_motor(uint8_t var0, uint8_t var1){
    uint8_t mida_txt;
    char text[50];

    lcd_setCursor(0,0);                                                 //esperem a que processi la lcd
    mida_txt= sprintf(text,"ML:%d%%   ", var0);                         //mida_text reb la mida de la array que conte la string que guardem a text. La string sera en decimal la "var0 - var1"
    lcd_print(text, mida_txt);                                          //printem la variable text
    lcd_setCursor(8,0);                                                 //esperem a que processi la lcd
    mida_txt = sprintf(text, "MR:%d%%   ",var1);                        //mida_text reb la mida de la array que conte la string que guardem a text. La string sera en decimal la "var0 - var1"
    lcd_print(text, mida_txt);                                          //printem la variable text
    lcd_setCursor(15,0);                                                //posem el cursor a dalt a la dreta

    if(velButtonF == 0) lcd_send_byte_dada(0xFF);                        //si estem parats, enviem la senyal dstop
    else{
        if(sentitButton == 1) lcd_send_byte_dada(0x7E);                     //si el sentit es endevant printem fletxa dreta
        else if(sentitButton == 0) lcd_send_byte_dada(0x7F);                //si el sentit es enderrera printem fletxa esquerra
    }

        lcd_setCursor(14,0);

}

void robot_print_LDR(uint8_t var2, uint8_t var3){
    uint8_t mida_txt;
    char text[50];

    mida_txt = sprintf(text, "ADCL:%d   ",var2);              //mida_text reb la mida de la array que conte la string que guardem a text. La string sera en decimal la "var0 - var1"
    lcd_print(text, mida_txt);                                          //printem la variable text
    lcd_setCursor(8,1);                                                       //esperem a que processi la lcd
    mida_txt = sprintf(text, "ADCR:%d     ",var3);              //mida_text reb la mida de la array que conte la string que guardem a text. La string sera en decimal la "var0 - var1"
    lcd_print(text, mida_txt);                                          //printem la variable text
    lcd_setCursor(16, 0);

}
