/*
 * cal.h
 *
 * Created: 2026-06-29 오후 4:24:26
 *  Author: kccistc
 */ 


#ifndef CAL_H_
#define CAL_H_

#define F_CPU 16000000UL  //16MHz
#include <avr/io.h>  // PORTA PORTB PORTD... IO관련 reg가 들어 있다.
#include <util/delay.h>  // _delay_ms _delay_us 등
#include <avr/interrupt.h> // sei, cli 등 함수
#include <string.h>

#define MAX_SIZE 100

typedef int element;
typedef struct
{
	int top;
	element data[MAX_SIZE];
} StackType;

void init_stack(StackType* s);
int is_empty(StackType* s);
int is_full(StackType* s);
void push(StackType* s, element item);
element pop(StackType* s);
element peek(StackType* s);
int get_priority(char op);
int infix_to_postfix(char* in, char* out);
int eval_postfix(char* out);

#endif /* CAL_H_ */