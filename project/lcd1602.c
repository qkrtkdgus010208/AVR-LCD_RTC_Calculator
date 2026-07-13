/*
 * lcd1602.c
 *
 * Created: 2026-07-01 오후 12:49:41
 *  Author: kccistc
 */ 

#include "lcd1602.h"

uint8_t lcd_len = 0;

void lcd_pulse_enable(void) // 하강 엣지에서 동작
{
	LCD_PORT_CONTROL |= 1 << E_PIN; // E를 High로
	_delay_us(1);
	LCD_PORT_CONTROL &= ~(1 << E_PIN); // E를 low로
	_delay_ms(1);
}

// 1바이트를 상위→하위 니블 순서로 2번 전송
void lcd_write_data(uint8_t data)
{
	if (data != ' ') lcd_len++;
	
	LCD_PORT_CONTROL |= 1 << RS_PIN; // 문자 출력에서 RS는 1
	
	LCD_PORT_DATA = data & 0xF0; // 상위 4비트
	lcd_pulse_enable();
	
	LCD_PORT_DATA = data << 4 & 0xF0; // 하위 4비트
	lcd_pulse_enable();
	
	if (lcd_len == 16) lcd_goto_xy(1, 0);
	
	_delay_ms(2);
}

void lcd_write_data_rtc(uint8_t data)
{
	LCD_PORT_CONTROL |= 1 << RS_PIN; // 문자 출력에서 RS는 1
	
	LCD_PORT_DATA = data & 0xF0; // 상위 4비트
	lcd_pulse_enable();
	
	LCD_PORT_DATA = data << 4 & 0xF0; // 하위 4비트
	lcd_pulse_enable();
	
	_delay_ms(2);
}

void lcd_erase_data(void)
{
	// 1행이 비어있다면 0행 15열을 삭제
	if (lcd_len == 16)
	{
		lcd_len--;
		lcd_goto_xy(0, 15);
		lcd_write_data(' ');
		lcd_goto_xy(0, 15);
		return;
	}
	
	lcd_write_command(0x10); // 커서를 왼쪽으로 1칸 이동 (0x10)
	lcd_write_data(' ');     // 그 자리에 공백을 써서 글자를 지움
	lcd_write_command(0x10); // 공백을 쓰며 커서가 전진했기데 다시 왼쪽으로 1칸 이동
	lcd_len--;
}

void lcd_write_command(uint8_t command)
{
	LCD_PORT_CONTROL &= ~(1 << RS_PIN); // 명령어 실행에서 RS는 0
	
	LCD_PORT_DATA = command & 0xF0; // 상위 4비트
	lcd_pulse_enable();
	
	LCD_PORT_DATA = command << 4 & 0xF0; // 하위 4비트
	lcd_pulse_enable();
	
	_delay_ms(2);
}

void lcd_clear(void)
{
	lcd_len = 0;
	lcd_write_command(COMMAND_CLEAR_DISPLAY);
	_delay_ms(2);
}

void init_lcd(void)
{
	_delay_ms(50); // 초기 구동 시간
	
	LCD_DDR_DATA |= 0xF0;   // 출력 모드 설정
	LCD_PORT_DATA = 0x00;
	
	LCD_DDR_CONTROL |= 1 << RS_PIN | 1 << E_PIN;
	
	lcd_write_command(0x02); // 4비트 모드 추가 명령
	lcd_write_command(COMMAND_4_BIT_MODE); // 4비트 모드
	
	// display on/off control
	// 화면 on, 커서 on, 커서 깜빡임 off
	uint8_t command = 0x08 | 1 << COMMAND_DISPLAY_ON_OFF_BIT | 1 << COMMAND_CURSOR_ON_OFF_BIT;
	lcd_write_command(command);
	
	lcd_clear(); // 화면 지음
	
	// Entry Mode Set
	// 출력 후 커서를 오른쪽으로 옮김, 즉 DDRAM의 주소가 증가하며 화면 이동은 없음
	lcd_write_command(0x06);
}

void lcd_write_string(char* string)
{
	uint8_t i;
	for (i = 0; string[i]; i++)	// 종료 문자를 만날 때까지
	lcd_write_data(string[i]); // 문자 단위 출력
}

void lcd_write_string_rtc(char* string)
{
	uint8_t i;
	for (i = 0; string[i]; i++)	// 종료 문자를 만날 때까지
	lcd_write_data_rtc(string[i]); // 문자 단위 출력
}

// DDRAM 주소 매핑: 1행=0x00~, 2행=0x40~
void lcd_goto_xy(uint8_t row, uint8_t col)
{
	row %= 2; // [0 ~ 1]
	col %= 16; // [0 ~ 15]
	
	// 첫째 라인 시작 주소는 0x00. 둘째 라인 시작 주소는 0x40
	uint8_t address = 0x40 * row + col;
	uint8_t command = 0x80 + address;
	
	lcd_write_command(command); // 커서 이동
}