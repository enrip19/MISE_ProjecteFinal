/*
 * lcdLib.h
 *
 *  Created on: 22 may. 2020
 *      Author: enric
 */
#include "msp.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef LIBRARY_LCDLIB_H_
#define LIBRARY_LCDLIB_H_

/* Defines de la LCD */
#define DADA_LCD 0x01
#define BACKLIGHT_LCD 0x08
#define E_LCD 0x04
#define CMD_LCD 0x00
#define LCD_ADDRESS 0x27
//instruccions de la lcd
#define INIT_LCD 0x30
#define BIT4_COMM 0x20
#define LINE2_5x8 0x28
#define LCD_OFF 0x08
#define LCD_CLEAR 0x01
#define INCR_MODE 0x06
#define LCD_ON 0x0F
#define UP_LINE 0x80
#define DOWN_LINE 0xC0
#define RTN_TO_HOME 0x02
#define GO_RIGHT 0x14

/* variables per la lcd */
uint8_t text[50];
uint8_t mida_txt;

/* funcions de la lcd*/
void lcd_send_nibble_cmd(uint8_t dada);
void lcd_send_byte_cmd(uint8_t dada);
void lcd_send_byte_dada(uint8_t dada);
void init_lcd(void);
void lcd_print(uint8_t *string, uint8_t mida);
void lcd_clear(void);
void lcd_setCursor(uint8_t col, uint8_t row);
void robot_print(char * _string, uint8_t col, uint8_t row);
void robot_print_var(uint8_t var0, uint8_t var1, uint8_t col, uint8_t row);
void robot_print_motor(uint8_t var0, uint8_t var1);
void robot_print_LDR(uint8_t var2, uint8_t var3);

#endif /* LIBRARY_LCDLIB_H_ */
