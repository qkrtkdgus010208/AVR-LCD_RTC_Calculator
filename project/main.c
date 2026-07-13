/*
 * 13.RTC_LCD_CAL.c
 *
 * Created: 2026-07-01 오전 10:05:29
 * Author : kccistc
 */ 
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> // sei, cli 등 함수
#include <stdio.h>
#include <string.h>

#include "button.h"
#include "uart0.h"
#include "keypad.h"
#include "queue.h"
#include "cal.h"
#include "button.h"
#include "lcd1602.h"
#include "i2c_ds1307.h"

void button_control(rtc_time_t* edit_t);
void check_button_lcd(void);
void check_button_ds1307(rtc_time_t *edit_t);
void lcd_control(char* lcd_buff, rtc_time_t* t, rtc_time_t* edit_t);
void lcd_cal_mode(void);
void lcd_rtc_mode(char* lcd_buff, rtc_time_t* t);
void lcd_edit_mode(char* lcd_buff, rtc_time_t* edit_t);
void init_main(char* lcd_buff, rtc_time_t* t, rtc_time_t* edit_t);
void init_timer0(void);
void init_buff(void);
void init_date_time(rtc_time_t* t);
void ds1307_set_time(rtc_time_t* t);
void ds1307_get_time(rtc_time_t* t);
uint8_t bcd_to_decimal(uint8_t bcd);
uint8_t decimal_to_bcd(uint8_t decimal);
void check_count(void);
void check_keypad_count(void);
void check_rtc_count(void);

volatile uint32_t ms_count = 0;
volatile uint32_t last_keypad_count = 0;
volatile uint32_t last_rtc_count = 0;

typedef enum { CAL, RTC, EDIT_YEAR, EDIT_MONTH, EDIT_DAY, EDIT_WEEK, EDIT_HOUR, EDIT_MIN, EDIT_SEC, END } mode_state;
mode_state mode = RTC;
uint8_t is_first_edit = 1;

uint8_t key_value;
char inbuff[MAX_SIZE];
char outbuff[MAX_SIZE];
int buff_index;

const char* WEEK_NAMES_ENG[] = { "", "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 6; // TCNT0 6 ~ 256 : 250개 펄스 count하기 위해
	ms_count++;
}

int main(void)
{
	// 지역 변수 선언
	char lcd_buff[64];
	rtc_time_t t, edit_t;
	
	// main 초기화 로직
	init_main(lcd_buff, &t, &edit_t);
	
	stdout = &OUTPUT; // printf가 동작할 수 있도록 stdout을 설정
	sei(); // 전역(대문) interrupt 허용
	
	// ds1307에 초기 시간 셋팅
	ds1307_set_time(&t);
	
    while (1)
    {
		// 각종 count 체크
		check_count();
		
		// 버튼 입력 제어
		button_control(&edit_t);
		
		// 모드에 따른 lcd 제어
		lcd_control(lcd_buff, &t, &edit_t);
    }
}

#pragma region button_control
void button_control(rtc_time_t* edit_t)
{
	if (mode == CAL) check_button_lcd();
	else check_button_ds1307(edit_t);
}

void check_button_lcd(void)
{
	if (get_button(BUTTON0, BUTTON0PIN))
	{
		key_value = '(';
		inbuff[buff_index++] = (char)key_value;
		lcd_write_data(key_value);
	}
	
	if (get_button(BUTTON1, BUTTON1PIN))
	{
		key_value = ')';
		inbuff[buff_index++] = (char)key_value;
		lcd_write_data(key_value);
	}
	
	if (get_button(BUTTON2, BUTTON2PIN))
	{
		if (buff_index > 0)
		{
			buff_index--;
			inbuff[buff_index] = '\0';
			lcd_erase_data();
		}
	}
	
	if (get_button(BUTTON3, BUTTON3PIN))
	{
		lcd_clear();
		init_buff();
	}
}

void check_button_ds1307(rtc_time_t *edit_t)
{
	if (edit_t == NULL) return;

	if (mode > RTC)
	{
		int8_t change = 0;
		if (get_button(BUTTON0, BUTTON0PIN)) change = -1;
		if (get_button(BUTTON1, BUTTON1PIN)) change = 1;
		
		if (change != 0)
		{
			switch (mode)
			{
				case EDIT_YEAR: // 년도 수정
					edit_t->year = (edit_t->year + change + 100) % 100; // 00~99 제한
					break;
				case EDIT_MONTH: // 월 수정
					edit_t->month = edit_t->month + change;
					if (edit_t->month < 1) edit_t->month = 12;
					if (edit_t->month > 12) edit_t->month = 1;
					break;
				case EDIT_DAY: // 일 수정
					edit_t->day = edit_t->day + change;
					if (edit_t->day < 1) edit_t->day = 31;
					if (edit_t->day > 31) edit_t->day = 1;
					break;
				case EDIT_WEEK: // 요일 수정
					edit_t->week = edit_t->week + change;
					if (edit_t->week < 1) edit_t->week = 7;
					if (edit_t->week > 7) edit_t->week = 1;
					break;
				case EDIT_HOUR: // 시 수정
					edit_t->hour = (edit_t->hour + change + 24) % 24;
					break;
				case EDIT_MIN: // 분 수정
					edit_t->min = (edit_t->min + change + 60) % 60;
					break;
				case EDIT_SEC: // 초 수정
					edit_t->sec = (edit_t->sec + change + 60) % 60;
					break;
			}
		}
	}
	
	if (get_button(BUTTON2, BUTTON2PIN))
	{
		lcd_clear();
		mode++;
		if (mode == END)
		{
			ds1307_set_time(edit_t);
			mode = RTC;
			is_first_edit = 1;
		}
	}
	
	if (get_button(BUTTON3, BUTTON3PIN))
	{
		lcd_clear();
		init_buff();
		mode = CAL;
		is_first_edit = 1;
		last_rtc_count = ms_count;
	}
}
#pragma endregion button_control

#pragma region lcd_mode_control
void lcd_control(char* lcd_buff, rtc_time_t* t, rtc_time_t* edit_t)
{
	// 큐에 데이터가 존재하면 계산기 모드
	if (queue_empty() != TRUE) lcd_cal_mode();
	
	if (mode == RTC) // RTC 모드
	{
		lcd_rtc_mode(lcd_buff, t);
	}
	else if (mode > RTC) // EDIT 모드
	{
		lcd_edit_mode(lcd_buff, edit_t);
	}
}

void lcd_cal_mode(void)
{
	// 최초 진입 시에만 초기화
	if (mode == RTC)
	{
		lcd_clear();
		init_buff();
	}
	
	// 키가 입력되었으므로 RTC 모드를 해제하고 카운터 리셋
	mode = CAL;
	last_rtc_count = ms_count;
	
	key_value = read_queue();
	inbuff[buff_index++] = (char)key_value;
	lcd_write_data(key_value);
	
	if (key_value == '=')
	{
		int check = infix_to_postfix(inbuff, outbuff);
		if (!check)
		{
			lcd_clear();
			lcd_write_string("error");
		}
		else
		{
			int result = eval_postfix(outbuff);
			char lcd_result_buff[16];
			sprintf(lcd_result_buff, "%d", result);
			lcd_write_string(lcd_result_buff);
		}
		init_buff();
	}
}

void lcd_rtc_mode(char* lcd_buff, rtc_time_t* t)
{
	if (ms_count >= 1000) // 1초마다 갱신
	{
		ms_count = 0;
		ds1307_get_time(t);
		
		// 첫 번째 줄에 날짜 출력
		lcd_goto_xy(0, 0);
		sprintf(lcd_buff, "-20%02d-%02d-%02d-%s-", t->year, t->month, t->day, WEEK_NAMES_ENG[t->week]);
		lcd_write_string_rtc(lcd_buff);
		
		// 두 번째 줄에 시간 출력
		lcd_goto_xy(1, 0);
		sprintf(lcd_buff, "Time: %02d:%02d:%02d", t->hour, t->min, t->sec);
		lcd_write_string_rtc(lcd_buff);
	}
}

void lcd_edit_mode(char* lcd_buff, rtc_time_t* edit_t)
{
	if (is_first_edit) // 최초 수정 모드 진입
	{
		ds1307_get_time(edit_t);
		is_first_edit = 0;
	}
	
	// 첫 번째 줄에 날짜 출력
	lcd_goto_xy(0, 0);
	switch (mode)
	{
		case EDIT_YEAR: // 년 수정 중
		sprintf(lcd_buff, "[20%02d]%02d-%02d-%s-", edit_t->year, edit_t->month, edit_t->day, WEEK_NAMES_ENG[edit_t->week]);
		break;
		case EDIT_MONTH: // 월 수정 중
		sprintf(lcd_buff, "-20%02d[%02d]%02d-%s-", edit_t->year, edit_t->month, edit_t->day, WEEK_NAMES_ENG[edit_t->week]);
		break;
		case EDIT_DAY: // 일 수정 중
		sprintf(lcd_buff, "-20%02d-%02d[%02d]%s-", edit_t->year, edit_t->month, edit_t->day, WEEK_NAMES_ENG[edit_t->week]);
		break;
		case EDIT_WEEK: // 요일 수정 중
		sprintf(lcd_buff, "-20%02d-%02d-%02d[%s]", edit_t->year, edit_t->month, edit_t->day, WEEK_NAMES_ENG[edit_t->week]);
		break;
		default:
		sprintf(lcd_buff, "-20%02d-%02d-%02d-%s-", edit_t->year, edit_t->month, edit_t->day, WEEK_NAMES_ENG[edit_t->week]);
		break;
	}
	lcd_write_string_rtc(lcd_buff);
	
	// 두 번째 줄에 시간 출력
	lcd_goto_xy(1, 0);
	switch (mode)
	{
		case EDIT_HOUR: // 시 수정 중
		sprintf(lcd_buff, "Time:[%02d]%02d:%02d", edit_t->hour, edit_t->min, edit_t->sec);
		break;
		case EDIT_MIN: // 분 수정 중
		sprintf(lcd_buff, "Time: %02d[%02d]%02d", edit_t->hour, edit_t->min, edit_t->sec);
		break;
		case EDIT_SEC: // 초 수정 중
		sprintf(lcd_buff, "Time: %02d:%02d[%02d]", edit_t->hour, edit_t->min, edit_t->sec);
		break;
		default:
		sprintf(lcd_buff, "Time: %02d:%02d:%02d", edit_t->hour, edit_t->min, edit_t->sec);
		break;
	}
	lcd_write_string_rtc(lcd_buff);
}
#pragma endregion mode_control

#pragma region init
void init_main(char* lcd_buff, rtc_time_t* t, rtc_time_t* edit_t)
{
	init_timer0();
	init_uart0();
	queue_init();
	init_keypad();
	init_button();
	init_lcd(); // 텍스트 LCD 초기화
	init_buff(); // 버퍼 초기화
	init_i2c(); // i2c 초기화
	init_date_time(t);
}

void init_timer0(void)
{
	TCNT0 = 6; // TCNT0 6 ~ 256 : 250개 펄스 count하기 위해
	
	TCCR0 &= ~(1 << CS02 | 1 << CS01 | 1 << CS00); // 0분주
	TCCR0 |= 1 << CS02 | 0 << CS01 | 0 << CS00; // 64분주
	TIMSK |= 1 << TOIE0; // TIMER0 Overflow INT
}

void init_buff(void)
{
	memset(inbuff, 0, MAX_SIZE);
	memset(outbuff, 0, MAX_SIZE);
	buff_index = 0;
}

void init_date_time(rtc_time_t* t)
{
	if (t == NULL) return;

	t->year  = 26;  // 26년
	t->month = 7;   // 7월
	t->day   = 1;   // 1일
	t->week  = 4;   // 수요일
	t->hour  = 17;  // 17시
	t->min   = 30;  // 30분
	t->sec   = 0;   // 00초
}
#pragma endregion init

#pragma region DS1307_API
void ds1307_set_time(rtc_time_t* t)
{
	i2c_start(); // i2c 시작
	i2c_write(DS1307_ADDRESS << 1); // i2c 주소 전송(write 모드)
	i2c_write(0x00); // RTC에 데이터를 기록할 메모리 시작 주소 전송
	
	i2c_write(decimal_to_bcd(t->sec));
	i2c_write(decimal_to_bcd(t->min));
	i2c_write(decimal_to_bcd(t->hour));
	i2c_write(decimal_to_bcd(t->week));
	i2c_write(decimal_to_bcd(t->day));
	i2c_write(decimal_to_bcd(t->month));
	i2c_write(decimal_to_bcd(t->year));
	
	i2c_stop(); // i2c 정지
}

void ds1307_get_time(rtc_time_t* t)
{
	i2c_start();
	i2c_write(DS1307_ADDRESS << 1);
	i2c_write(0x00);
	i2c_stop();
	
	i2c_start();
	i2c_write(DS1307_ADDRESS << 1 | 1); // i2c 주소 전송(read 모드)
	
	t->sec = bcd_to_decimal(i2c_read_ack());
	t->min = bcd_to_decimal(i2c_read_ack());
	t->hour = bcd_to_decimal(i2c_read_ack());
	t->week = bcd_to_decimal(i2c_read_ack());
	t->day = bcd_to_decimal(i2c_read_ack());
	t->month = bcd_to_decimal(i2c_read_ack());
	t->year = bcd_to_decimal(i2c_read_nack());
	
	i2c_stop();
}
#pragma endregion DS1307_API

#pragma region conversion
uint8_t bcd_to_decimal(uint8_t bcd) // bcd 형식 -> 이진수 형식
{
	return (bcd >> 4) * 10 + (bcd & 0x0F);
}

uint8_t decimal_to_bcd(uint8_t decimal) // 이진수 형식 -> bcd 형식
{
	return (decimal / 10) << 4 | decimal % 10;
}
#pragma endregion conversion

#pragma region check_count
void check_count(void)
{
	check_keypad_count();
	check_rtc_count();
}

void check_keypad_count(void)
{
	if (ms_count - last_keypad_count >= 60)
	{
		last_keypad_count = ms_count;
		uint8_t key_data = keypad_scan();
		if (key_data) insert_queue(key_data); // keypad를 check해서 눌린 것이 있으면 circular queue에 저장
	}
}

void check_rtc_count(void)
{
	if (ms_count - last_rtc_count >= 3000)
	{
		last_rtc_count = ms_count;
		if (mode == CAL) mode = RTC;
	}
}
#pragma endregion check_count