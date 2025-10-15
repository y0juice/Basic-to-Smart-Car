# 4) TM_TimeBase
기능) CNT 제어로 LED blink
![EXIT](https://github.com/user-attachments/assets/9d39c5b6-b0d3-4046-b6e5-d343873ea304)

### (1) Pinout & Configuration

#### 01) TIM3 설정
- 총 5가지 타이머 존재(RTC, TIM1, TIM2, TIM3, TIM4)<br>
##### <STM32 타이머 + RTC 비교 & 추천 사용 상황>

| 항목 / 사용 상황 | TIM1 | TIM2 | TIM3 / TIM4 | RTC |
|--------------------|------|------|---------------|-----|
| **타이머 종류** | 고급 타이머 (Advanced) | 일반 타이머 (32bit) | 일반 타이머 (16bit) | 실시간 시계 (Real‑Time Clock) |
| **비트 수** | 16비트 | **32비트** | 16비트 | 32비트 (날짜/시간) |
| **정밀도 / 시간 범위** | 매우 높음 (μs 단위) / 짧음 | 높음 (μs~ms) / 매우 김 | 높음 (μs~ms) / 중간 | 낮음 (1초 단위) / 매우 김 (수년 단위) |
| **PWM 출력**<br>(duty 비 조절) | ✅ 고급 PWM (Complementary, 데드타임) | ✅ 기본 PWM | ✅ 기본 PWM | ❌ 없음 |
| **데드타임 제어** | ✅ 있음 | ❌ 없음 | ❌ 없음 | ❌ 없음 |
| **슬립모드 동작 / 전원 꺼짐 유지** | ❌ / ❌ | ❌ / ❌ | ❌ / ❌ | ✅ 가능 / ✅ (백업 배터리 사용 시) |
| **날짜/시간 기능** | ❌ | ❌ | ❌ | ✅ (연/월/일/시/분/초) |
| **알람 기능** | ❌ | ❌ | ❌ | ✅ 날짜/시간 기반 알람 가능 |
| **추천 사용 상황** | 고급 PWM / 모터 제어 | 긴 시간 측정 | LED 깜빡이기, 서보 PWM, 기본 타이밍 | 실제 시계, 알람, 절전 모드 복귀, 시간 유지 |

=>**LED 깜빡이기** 기능을 구현하려면, 간단한 PWM 출력이 가능한 **TIM3**을 사용하는 것이 적합






- stm32<br>
<img width="926" height="670" alt="image" src="https://github.com/user-attachments/assets/2d2ea926-5483-45f9-b841-2c92d7576373" /><br>
<img width="1538" height="654" alt="image" src="https://github.com/user-attachments/assets/807db4b7-a69a-4b80-a04d-a8c236d1fa47" />

#### 02) RCC(Reset & Clcok Contorl)
<img width="1105" height="552" alt="image" src="https://github.com/user-attachments/assets/fea9eacc-5b89-47b8-8bb4-f71d4eb84576" /><br>


### (2) 코드작성
- main.c 추가 코드

```c
//버튼(외부 인터럽트)-> 콜백함수 실행 => 핀번호 확인(B1_Pin(13) -> LED 토글(누르면 켜지다가 다음번누를때 꺼짐)
void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
	case B1_Pin://B1을 눌렀을 때
		HAL_GPIO_TogglePin (LD2_GPIO_Port, LD2_Pin);// LED가 켜져잇으면 끄고, 켜져 있으면 켠다: Toggle
		break;

	default:// 그 외 버튼은 무
		;
	}
}
```
=>B1으로 LED Toggle
