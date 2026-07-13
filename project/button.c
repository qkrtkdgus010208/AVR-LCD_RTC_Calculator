/*
 * button.c
 *
 * Created: 2026-06-10 오후 1:25:24
 *  Author: kccistc
 */ 

#include "button.h" // 현재 디렉토리에 있는 파일을 포함

// 버튼 초기화 방향 설정(입력으로)
void init_button(void)
{
	BUTTON_DDR &= ~(1 << BUTTON0PIN | 1 << BUTTON1PIN | 1 << BUTTON2PIN | 1 << BUTTON3PIN);
}

// 예) BUTTON 3
// button을 눌렀다 떼면 1을 리턴
// button이 idle 상태면 0을 리턴
int get_button(int button_num, int button_pin)
{
	// 지역변수에 static을 선언하면 함수를 빠져나와 다시 들어가도 이전 값을 유지
	static unsigned char button_status[BUTTON_NUMBER] = 
	{
		BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE
	};
	
	// 1. button을 읽는다.
	int current_state = BUTTON_PIN & (1 << button_pin);
	
	// 2. 버튼 상태 check
	if (current_state && button_status[button_num] == BUTTON_RELEASE) // 버튼을 처음 눌려진 상태
	{
		_delay_ms(15); // noise가 지나가기를 기다림
		button_status[button_num] = BUTTON_PRESS;
		return 0; // 아직은 완전히 눌렀다 뗀 상태가 아님
	}
	else if (button_status[button_num] == BUTTON_PRESS && current_state == BUTTON_RELEASE)
	{
		// 버튼이 이전에 눌려진 상태였으며 지금은 떼어진 상태
		button_status[button_num] = BUTTON_RELEASE; // 다음 버튼을 체크하기 위해 초기화
		_delay_ms(15); // noise가 지나가기를 기다림
		return 1; // 완전히 1번 눌렀다 뗀 상태로 인정
	}
	
	return 0; // 버튼이 open 상태
}
