# 4) TM_TimeBase
기능) CNT 제어로 LED blink<br>
![4  TM_TimeBase](https://github.com/user-attachments/assets/cb6b9cf4-2763-4236-ad72-e6e0721a3f6e)

### (1) Pinout & Configuration

#### 01) RCC(Reset & Clcok Contorl)
<img width="1105" height="552" alt="image" src="https://github.com/user-attachments/assets/fea9eacc-5b89-47b8-8bb4-f71d4eb84576" /><br>


#### 02) TIM3 설정
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
___

=>**LED 깜빡이기** 기능을 구현하려면, 간단한 PWM 출력이 가능한 **TIM3**을 사용하는 것이 적합
<img width="1214" height="619" alt="image" src="https://github.com/user-attachments/assets/00308e09-e77c-4539-9e94-c68844b1ed18" /><br>
- TIM의 parameter 조절 전, Clock and Configuration 탭에서 타이머에 공급되는 클럭 확인: **64(MHz)**<br>
  
<img width="831" height="616" alt="image" src="https://github.com/user-attachments/assets/e921ecc3-0c73-46c4-9ceb-0935cbb2ef81" /><br>
- Prescaler와 Counter Period 설정값에 의해 타이머 인터럽트의 발생 주기 결정
  => Prescaler: 입력 클럭을 나누는 값, prescaler=n 이라면 n개의 클럭이 입력될 때마다 1개의 클럭 출력<br>
  => Counter Period: 카운터가 0부터 카운트할 최댓값<br>
- Prescarler = 63, Counter Period 999<br>
  => Prescarler를 64로 설정: 타이머 공급(64,000,000Hz)/64= 1,000,000 Hz 클럭 공급<br>
                           : 1초에 1,000,000 클럭인가 = 1clk 당, 1/1,000,000초 소요<br>
  => Counter Period를 1000으로 설정: 클럭을 1,000번 카운트 할 때마다 타이머 인터럽트 발생<br>
                                   : 인터럽트 주기 1/1,000,000초 * 1,000 = 1/1,000초 = 1ms<br>
								     ***(1s가 필요하다면 interrupt 1,000이 발생할 때마다 처리)***

  #### 03) interrupt 설정
<img width="1845" height="428" alt="image" src="https://github.com/user-attachments/assets/bb8697be-b09f-4004-b8ed-b5fcd2deab76" />

___
#### Generate Code ####
___
  #### 03) 코드작성
  -main.c 추가 코드
```c
/* USER CODE BEGIN PV */
volatile int gTimerCnt;
/* USER CODE END PV */
```
=> volatile을 붙이면 항상 메모리에 읽고 쓰도록 함( interrupt에서 바뀐 값이 즉시 반영되도록)

```c
  /* USER CODE BEGIN 2 */
if (HAL_TIM_Base_Start_IT (&htim3) != HAL_OK)// 타이머 시작 실패 시 처리
{
	Error_Handler ();// 오류 처리 루틴
}
  /* USER CODE END 2 */
```
=> HAL_TIM_Base_Start_IT(&htim3): TIM3 타이머를 interrupt 모드로 시작

```c
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim)
{
	gTimerCnt++;
	if(gTimerCnt ==1000)//clk=lms -> gTimerCnt==1000 1초당 한번 깜빡임
	{
		gTimerCnt = 0;
		HAL_GPIO_TogglePin (LD2_GPIO_Port, LD2_Pin);
	}
}
```
=> void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim): 타이머가 주기마다 호출<br>
=> gTimerCnt++; : interrupt 발생 시 카운터 증가. volatile int gTimerCnt로 선언<br>
=> if(gTimerCnt ==1000): CNT clk 64M, Prescaler=64, CNT period= 1000 -> **인터럽트 주기 = 1/1,000초 = 1ms *1000번 = 1s** <br>
