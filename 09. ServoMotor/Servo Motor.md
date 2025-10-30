 # 9. Servo Motor
**기능) 타이머로 SG90 Servo 제어**<br>
<br>

__
### (1) 이론 설명 ###
<img width="300" height="250" alt="image" src="https://github.com/user-attachments/assets/7077f917-8ec3-4781-9efb-d953e937d311" /><br>
- 서버모터 날게 너무 꽉 조이면 안돼!!(고장 위험)
- 타이머를 이용해 일정한 주기의 PWM 신호를 생성 -> 원하는 각도 조절
-  SG90 서보 요구 PWM 주기 = 20 ms (50 Hz)<br>
  
<PWM><br>

| 용어 | **의미** | **영향** |
|------|-----------|-----------|
| **PWM 주기 (Period)** | 한 번의 ON/OFF 전체 시간 (예: 20ms) | 전체 속도 |
| **PWM 딜레이 (Phase Delay)** | PWM 시작 시점의 지연 (특정 위상에서 시작) | 모터 동기 제어용 |
| **PWM 듀티비 (Duty)** | High 구간 비율 (%) | 속도, 각도 결정 |






### (2) Pinout & Configuration

#### 01) RCC(Reset & Clcok Contorl)
<img width="1105" height="552" alt="image" src="https://github.com/user-attachments/assets/fea9eacc-5b89-47b8-8bb4-f71d4eb84576" /><br>


#### 02) TIM1 설정
<STM 타이머 종류>
 
| 구분 | **타이머 이름** | **종류** | **특징** |
|------|------------------|-----------|-----------|
| **TIM1** | Advanced-control Timer | **고급 제어용** | PWM + Dead time + Complementary Output (모터 제어용) |
| **TIM2** | General-purpose Timer | **일반용** | 32-bit 카운터 (시간 측정/기본 PWM용) |
| **TIM3** | General-purpose Timer | **일반용** | 16-bit, 여러 PWM 채널 제어 가능 |
| **TIM4** | General-purpose Timer | **일반용** | 16-bit, PWM이나 인터럽트 타이밍용 |

- TIM2 & TIM3 사용<br>
<img width="1420" height="695" alt="image" src="https://github.com/user-attachments/assets/2ccf113c-b34d-45a2-8d5d-679159b1e789" />
- 기본조건 **(타이머 클럭= 64MHx/ Prescaler= 1280-1/ Period= 1000-1)**

- Prescaler와 Counter Period 설정값에 의해 타이머 인터럽트의 발생 주기 결정
  => Prescarler를1279로 설정: 타이머 공급(64,000,000Hz)/1280= **50,000 Hz 클럭** 공급<br>
                           : 1초에 50,000 클럭인가 = 1clk 당, 1/50,000(0.02:20us)초 소요<br>
  => Counter Period를 999으로 설정: 인터럽트 주기 1/50,000초 * 1000(1000 clk 후 인터럽트 발생) <br>
- PWM 주기= 20ms(50Hz) : SG90 서보 요구사항과 일치<br>

<펄스폭: CCR 값으로 각도 제어>
→ PWM 한 주기 = 20 ms<br>
→ 한 클럭(타이머 1 tick) = 20 µs<br>
| **펄스 폭 (High 시간)** | **Duty(%)** | **의미** |
|------------------------|-------------|-----------|
| 1ms(최소) | 1ms/20us=50 | CCR=50 | 
| 1.5ms(중간) | 1.5ms/20us=75 | CCR=75 |
| 2ms(최대) | 2ms/20us=100 | CCR=100 |<br>
- 0° → 1 ms → CCR = 50
- 90° → 1.5 ms → CCR = 75
- 180° → 2 ms → CCR = 100

  
  #### 03) 핀 패정
<img width="1196" height="592" alt="image" src="https://github.com/user-attachments/assets/c3d790b0-40c5-413a-a9be-dbc11ef0536a" />
- 모터 2개활성화(TIM에서 channel 형성하면 자동으로 생김)<br>
(TIM2_CH1 - PA0, TIM3_CH1 - PA6)<br>
___
#### Generate Code ####
___
  #### (3) 코드작성
  -main.c 추가 코드
```c
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */
```

```c
#define MAX 100  // 2.5ms pulse width (최대 각도:180)
#define MIN 50   // 0.5ms pulse width (최소 각도:0)
#define CENTER 75 // 1.5ms pulse width (중앙 각도:90)
#define STEP 1
/* USER CODE END PD */

/* USER CODE BEGIN PV */
uint8_t ch;
uint8_t pos_pan = 75;
uint8_t pos_tilt = 75;
/* USER CODE END PV */
```
```c
/* USER CODE BEGIN 0 */
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  if (ch == '\n')
    HAL_UART_Transmit (&huart2, (uint8_t*) "\r", 1, 0xFFFF);
  HAL_UART_Transmit (&huart2, (uint8_t*) &ch, 1, 0xFFFF);

  return ch;
}
/* USER CODE END 0 */
```

```c
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

  // 초기 위치 설정
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pos_pan);
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pos_tilt);

  printf("Servo Control Ready\r\n");
  printf("Commands: w(up), s(down), a(left), d(right), i(center)\r\n");
  /* USER CODE END 2 */
```
```c
 /* USER CODE BEGIN WHILE */
  while (1)
  {
    if(HAL_UART_Receive(&huart2, &ch, sizeof(ch), 10) == HAL_OK)
    {
      if(ch == 's')
      {
        printf("Down\r\n");
        if(pos_tilt + STEP <= MAX)
          pos_tilt = pos_tilt + STEP;
        else
          pos_tilt = MAX;
      }
      else if(ch == 'w')
      {
        printf("Up\r\n");
        if(pos_tilt - STEP >= MIN)
          pos_tilt = pos_tilt - STEP;
        else
          pos_tilt = MIN;
      }
      else if(ch == 'a')
      {
        printf("Left\r\n");
        if(pos_pan + STEP <= MAX)
          pos_pan = pos_pan + STEP;
        else
          pos_pan = MAX;
      }
      else if(ch == 'd')
      {
        printf("Right\r\n");
        if(pos_pan - STEP >= MIN)
          pos_pan = pos_pan - STEP;
        else
          pos_pan = MIN;
      }
      else if(ch == 'i')
      {
        printf("Center\r\n");
        pos_pan = CENTER;
        pos_tilt = CENTER;
      }

      // PWM 듀티 사이클 업데이트
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pos_pan);
      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pos_tilt);

      printf("Pan: %d, Tilt: %d\r\n", pos_pan, pos_tilt);

      HAL_Delay(50); // 서보 응답 시간
    }

    /* USER CODE END WHILE */
```
=> TIM2 에 연결된게 pan(a/d)조작, TIM3 연결된게 pan(w/s)로 조절<br>


