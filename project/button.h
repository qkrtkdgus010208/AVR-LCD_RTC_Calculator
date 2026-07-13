/*
 * button.h
 *
 * Created: 2026-06-10 오전 11:44:57
 *  Author: kccistc
 */ 

#define F_CPU 16000000UL // 16MHz
#include <avr/io.h>	// PORTA, PORTD 등의 I/O register 들이 들어있다.
#include <util/delay.h> // _delay_ms, _delay_us 등의 함수가 들어있다.

#define BUTTON_DDR DDRD
#define BUTTON_PIN PIND // PORTD를 읽는 register 5v: 1, 0v: 0

#define BUTTON0PIN 3 // PORTD.3
#define BUTTON1PIN 4 // PORTD.4
#define BUTTON2PIN 5 // PORTD.5
#define BUTTON3PIN 6 // PORTD.6

#define BUTTON0 0 // PORTD.3의 가상 index (sw번호)
#define BUTTON1 1 // PORTD.3의 가상 index (sw번호)
#define BUTTON2 2 // PORTD.3의 가상 index (sw번호)
#define BUTTON3 3 // PORTD.3의 가상 index (sw번호)
#define BUTTON_NUMBER 4 // 버튼 개수

#define BUTTON_PRESS 1 // 버튼을 누르면 high (active-high)
#define BUTTON_RELEASE 0 // 버튼을 뗀 상태 (low)

void init_button(void);
int get_button(int button_num, int button_pin);