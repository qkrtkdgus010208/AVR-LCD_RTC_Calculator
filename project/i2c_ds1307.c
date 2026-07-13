/*
 * i2c_ds1307.c
 *
 * Created: 2026-07-01 오후 3:44:56
 *  Author: kccistc
 */ 

#include "i2c_ds1307.h"

void init_i2c(void)
{
	TWBR = TWBR_VALUE; // I2C 클록 주파수 설정: 100KHz
	TWSR = 0x00;
}

void i2c_start(void)
{
	TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN;
	while (!(TWCR & 1 << TWINT));
}

void i2c_stop(void)
{
	TWCR = 1 << TWINT | 1 << TWSTO | 1 << TWEN;
	_delay_us(10);
}

void i2c_write(uint8_t data)
{
	TWDR = data;
	TWCR = 1 << TWINT | 1 << TWEN;
	while (!(TWCR & 1 << TWINT));
}

uint8_t i2c_read_ack(void)
{
	TWCR = 1 << TWINT | 1 << TWEN | 1 << TWEA; // TWEA: 1, ACK 자동 전송
	while (!(TWCR & 1 << TWINT));
	return TWDR;
}

uint8_t i2c_read_nack(void)
{
	TWCR = 1 << TWINT | 1 << TWEN; // TWEA: 0, NACK 자동 전송
	while (!(TWCR & 1 << TWINT));
	return TWDR;
}