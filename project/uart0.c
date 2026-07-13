/*
 * uart0.c
 *
 * Created: 2026-06-16 오전 9:57:56
 *  Author: kccistc
 */ 

#include "uart0.h"

volatile int u_rear = 0;  // interrupt에서 queue에 data를 저장하는 위치값
volatile int u_front = 0;  // pc_command_processing에서 가져가는 위치값
volatile unsigned char rx_buff[QUEUE_SIZE][QUEUE_LENGTH];

// 278p 표12-3
// PC로부터 1byte가 들어오면 자동적으로 이곳으로 진입한다.
// 예) led_all_on\n 이면 11번 이것으로 진입한다.
ISR(USART0_RX_vect)
{
	volatile uint8_t data;
	static volatile int i = 0;
	
	data = UDR0; // UDR0의 내용이 data에 복사된 후 UDR0내용은 빈 상태로 된다.
	
	// queue full 상태
	if ((u_rear + 1) % QUEUE_SIZE == u_front % QUEUE_SIZE) return;
	
	if (data == '\n' || data == '\r')
	{
		rx_buff[u_rear][i] = '\0'; // 문장의 끝인 NULL을 넣는다.
		u_rear = (u_rear + 1) % QUEUE_SIZE; // 0 ~ 9
		i = 0; // 다음 string을 저장하기 위해서 i를 0으로 만든다.
	}
	else
	{
		rx_buff[u_rear][i++] = data;
	}
}

/*
1. 전송속도: 9600bps
2. start/stop 설정
3. RX(수신): interrupt로 설정
4. TX(송신): polling으로 설정
*/
void init_uart0(void)
{
	// 1. 전송속도: 9600bps
	UBRR0H = 0x00;
	UBRR0L = 207; // 9600bps
	UCSR0A |= 1 << U2X0; // 2배속 설정 (sampling 8)
	// UART0를 송신, 수신이 다 가능하고 RX가 INT가 가능하도록 설정한다.
	UCSR0B |= 1 << RXEN0 | 1 << TXEN0 | 1 << RXCIE0;
}

// UART0로 1byte를 전송하는 함수
int UART0_transmit(char data, FILE *stream)
{
	while (!(UCSR0A & 1 << UDRE0)); // data가 송신 중이면 끝날 때까지 기다림
	
	UDR0 = data; // HW 전송 register에 data를 송신한다.
	
	return 0;
}

/*void pc_command_ds1302(t_ds1302* ds1302)
{
	if (front != rear) // data가 rx_buff에 존재하는지
	{
		printf("%s\n", rx_buff[front]);
		
		// setrtc가 일치하는지 확인
		if (strncmp((char *)rx_buff[front], "setrtc", 6) == 0)
		{
			// rx_buff[front]의 6번 인덱스부터 260629102730 시작됨
			uint8_t* p = &rx_buff[front][6];
			
			// 문자가 정상적인 숫자 범위(0~9)인지 검사
			// '0' 이상 '9' 이하의 문자가 12자리 연속되는지 확인
			int is_valid = 1;
			for (int i = 0; i < 12; i++)
			{
				if (p[i] < '0' || p[i] > '9')
				{
					is_valid = 0;
					break;
				}
			}
			
			if (is_valid)
			{
				ds1302->year    = ((p[0] - '0') * 10) + (p[1] - '0');
				ds1302->month   = ((p[2] - '0') * 10) + (p[3] - '0');
				ds1302->date    = ((p[4] - '0') * 10) + (p[5] - '0');
				ds1302->hours   = ((p[6] - '0') * 10) + (p[7] - '0');
				ds1302->minutes = ((p[8] - '0') * 10) + (p[9] - '0');
				ds1302->seconds = ((p[10] - '0') * 10) + (p[11] - '0');
				
				init_ds1302(ds1302);
				
				printf("RTC Set complete\n");
			}
			else
			{
				printf("RTC Set error\n");
			}
		}
		
		front = (front + 1) % QUEUE_SIZE;
	}
}*/