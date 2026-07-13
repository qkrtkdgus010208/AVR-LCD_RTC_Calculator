# 📝 AVR LCD 시계 & 계산기 프로젝트

---

# 📚 목차

- 개요
- 외관 사진
- Part List
- 회로도
- FSM
- 오실로스코프 동작 파형 측정
- 동작 영상
- 구현 후기

---

# 개요

## 프로젝트 목적
### 텍스트 LCD 제어 원리 습득
- 4-bit 인터페이스 초기화 및 명령어 체계를 직접 구현하며 타이밍 기반 하드웨어 제어를 학습

### I2C(2-wire) 프로토콜 실습
- DS1307 RTC와의 통신을 통해 START/ACK/STOP 등 I2C 버스 시나리오를 ATmega128 하드웨어 I2C 통신 방법으로 직접 구현

### 문자열 파싱 알고리즘 적용
- 중위 표기법 수식을 후위 표기법 수식으로 변환하여 결과값을 도출하는 계산기를 구현

### 최종 동작 요약
<img width="355" height="209" alt="image" src="https://github.com/user-attachments/assets/355adfa5-58b0-4767-839e-1292d2b0b914" />

## 개발 환경
<img width="831" height="253" alt="image" src="https://github.com/user-attachments/assets/262e12b5-dd91-43eb-9316-5f16d9d093d0" />

---

### 외관 사진
<img width="1674" height="1256" alt="image" src="https://github.com/user-attachments/assets/307c6221-13e3-4578-80f4-0234535c0a96" />

---

### Part List
<img width="2738" height="941" alt="image" src="https://github.com/user-attachments/assets/49e3d0e0-af65-4968-93ea-1e0bf9362f1e" />

---

### 회로도
<img width="2308" height="1245" alt="image" src="https://github.com/user-attachments/assets/89fd6543-541d-40b6-8427-c1b647756754" />

---

### FSM
<img width="805" height="366" alt="image" src="https://github.com/user-attachments/assets/72930b53-1166-4344-a4e9-a79ec7a69128" />

---

### 오실로스코프 동작 파형 측정
<img width="676" height="242" alt="오실로스코프 동작 파형 측정" src="https://github.com/user-attachments/assets/113054b8-fffa-4657-a7d2-c38babbad328" />

---

### 동작 영상
|동작 영상 - 시계|동작영상 - 계산기|
|:---:|:---:|
| [![동작 영상 - 시계](https://img.youtube.com/vi/vxd3baSa5qE/hqdefault.jpg)](https://youtu.be/vxd3baSa5qE) | [![동작영상 - 계산기](https://img.youtube.com/vi/WpWk6BgvaMQ/hqdefault.jpg)](https://youtu.be/WpWk6BgvaMQ) |

---

### 구현 후기

## I2C 통신 안정화
- DS1307의 SDA/SCL을 ATmega128A에 바로 연결 시 Floating 상태가 되어 통신이 불가능해지는 현상 발생 -> 풀업 저항 연결

## LCD 4bit 초기화
- 데이터 시트의 초기화하는 대기시간과 코드를 정확히 지키지 않으면 화면이 깨지는 현상을 겪음 → 데이터 시트의 Figure 12 초기화 시퀀스를 그대로 재현하여 해결

## LCD 밝기 조절
- 초기 VEE를 그라운드에 연결하니 LCD에 글자가 잘 나오지 않는 현상 발생 -> VEE를 가변 저항에 연결

## 데이터 시트의 중요성
- 대부분의 문제는 데이터 시트를 정확하게 파악하지 못한 것에서 발생 -> 데이터 시트의 중요성을 다시금 체감
