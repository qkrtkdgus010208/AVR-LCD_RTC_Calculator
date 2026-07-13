/*
 * cal.c
 *
 * Created: 2026-06-29 오후 4:24:18
 *  Author: kccistc
 */ 

#include "cal.h"

void init_stack(StackType* s)
{
	s->top = -1;
}

int is_empty(StackType* s)
{
	return s->top == -1;
}

int is_full(StackType* s)
{
	return (s->top == (MAX_SIZE - 1));
}

void push(StackType* s, element item)
{
	if (is_full(s))
		printf("스택 포화 상태\n");
	else
		s->data[++(s->top)] = item;
}

element pop(StackType* s)
{
	if (is_empty(s))
		printf("스택 공백 상태\n");
	else
		return s->data[(s->top)--];
}

element peek(StackType* s)
{
	if (is_empty(s))
		printf("스택 공백 상태\n");
	else
		return s->data[s->top];
}

int get_priority(char op)
{
	switch (op)
	{
		case '(':
			return 0;
		case '+':
		case '-':
			return 1;
		case '*':
		case '/':
		case '%':
			return 2;
		default:
			return -1;
	}
}

// 중위 표기식을 후위 표기식으로 변환
int infix_to_postfix(char* in, char* out)
{
	StackType temp; // 스택 선언
	init_stack(&temp); // 스택 초기화
	int length = strlen(in); // 문자열의 길이를 구함
	int open = 0;
	int close = 0;

	for (int i = 0; i < length; i++) // 문자열의 길이만큼 반복
	{
		if (*in == '(') // 열린 괄호일 때
		{
			open++; // 열린 괄호 갯수 증가
			push(&temp, *in); // 스택에 삽입
		}
		else if (*in == ')') // 닫힌 괄호일 때
		{
			close++; // 닫힌 괄호 갯수 증가
			// 스택이 비어있지 않고, 열린 괄호를 만날 때까지 반복
			while (!is_empty(&temp) && peek(&temp) != '(')
			{
				*out++ = ' '; // 공백 삽입
				*out++ = pop(&temp); // 스택에 들어있던 연산자 삽입
			}
			pop(&temp); // '(' 삭제
		}
		else if (*in == '*' || *in == '/' || *in == '%' || *in == '+' || *in == '-') // 연산자일 때
		{
			// 스택이 비어있지 않고, 연산자 우선순위가 작거나 같으면 반복
			while (!is_empty(&temp) && get_priority(*in) <= get_priority(peek(&temp)))
			{
				*out++ = ' '; // 공백 삽입
				*out++ = pop(&temp); // 스택에 들어있던 연산자 삽입
			}
			*out++ = ' '; // 현재 연산자를 쓸 때 공백 삽입
			push(&temp, *in);
		}
		else if (*in >= '0' && *in <= '9') // 숫자일 때
		{
			*out++ = *in; // 숫자 삽입
		}

		in++; // 다음 문자로 이동
	}

	while (is_empty(&temp) != 1)
	{
		*out++ = pop(&temp);
	}

	*out = '\0';
	
	return open == close;
}

// 후위 표기식을 계산하여 최종 결과값을 반환
int eval_postfix(char* out)
{
	StackType eval_stack;
	init_stack(&eval_stack);

	while (*out != '\0')
	{
		if (*out == ' ') // 공백은 패스
		{
			out++;
			continue;
		}

		if (*out >= '0' && *out <= '9') // 숫자를 만났을 때
		{
			int num = 0;

			while (*out >= '0' && *out <= '9') // 숫자면 계속 반복하며 조립 ((두 자릿수 이상 처리를 위함)
			{
				num = (num * 10) + (*out - '0');
				out++;
			}

			push(&eval_stack, num); // 조립한 숫자를 스택에 삽입

			out--; // 반복문 끝에서 이동하므로 중복되지 않도록 이전 문자로 이동
		}
		else if (*out == '+' || *out == '-' || *out == '*' || *out == '/' || *out == '%') // 연산자면 스택에서 2개를 꺼내 계산
		{
			int op2 = pop(&eval_stack);
			int op1 = pop(&eval_stack);

			switch (*out)
			{
				case '+': push(&eval_stack, op1 + op2); break;
				case '-': push(&eval_stack, op1 - op2); break;
				case '*': push(&eval_stack, op1 * op2); break;
				case '/': push(&eval_stack, op1 / op2); break;
				case '%': push(&eval_stack, op1 % op2); break;
			}
		}

		out++; // 다음 문자로
	}

	return pop(&eval_stack); // 결과 반환
}