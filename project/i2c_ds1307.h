/*
 * i2c_ds1307.h
 *
 * Created: 2026-07-01 오후 3:44:50
 *  Author: kccistc
 */ 


#ifndef I2C_DS1307_H_
#define I2C_DS1307_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> // sei, cli 등 함수
#include <stdio.h>
#include <string.h>

#define I2C_FREQUENCY 100000UL
#define TWBR_VALUE (F_CPU / I2C_FREQUENCY - 16) / 2 // 100KHz

#define DS1307_ADDRESS 0x68

#define I2C_SCl PD0
#define I2C_SDA PD1

typedef struct {
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t week;
	uint8_t day;
	uint8_t month;
	uint8_t year;
} rtc_time_t;

void init_i2c(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(uint8_t data);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);

#endif /* I2C_DS1307_H_ */