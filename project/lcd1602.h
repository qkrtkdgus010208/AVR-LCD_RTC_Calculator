/*
 * lcd1602.h
 *
 * Created: 2026-07-01 오후 12:49:47
 *  Author: kccistc
 */ 


#ifndef LCD1602_H_
#define LCD1602_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> // sei, cli 등 함수
#include <stdio.h>
#include <string.h>

#define LCD_DDR_DATA		DDRF
#define LCD_DDR_CONTROL		DDRC

#define LCD_PORT_DATA		PORTF
#define LCD_PORT_CONTROL	PORTC

#define LCD_PIN_DATA		PINF
#define LCD_PIN_CONTROL		PINC

#define RS_PIN		0
#define E_PIN		1

#define COMMAND_CLEAR_DISPLAY	0x01
#define COMMAND_4_BIT_MODE		0x28

#define COMMAND_DISPLAY_ON_OFF_BIT 2
#define COMMAND_CURSOR_ON_OFF_BIT 1
#define COMMAND_BLINK_ON_OFF_BIT 0

void lcd_clear(void);
void lcd_pulse_enable();
void lcd_write_data(uint8_t data);
void lcd_write_data_rtc(uint8_t data);
void lcd_erase_data(void);
void lcd_write_command(uint8_t command);
void init_lcd(void);
void lcd_write_string(char* string);
void lcd_write_string_rtc(char* string);
void lcd_goto_xy(uint8_t row, uint8_t col);

#endif /* LCD1602_H_ */