# 6. HC-SR04
**기능) 초음파센서로 거리측정**<br>
<img width="780" height="614" alt="image" src="https://github.com/user-attachments/assets/e4091a1d-72b3-49b4-b9c8-4dd4335f12ec" />
<br>

__
### (1) 이론 설명 ###
<img width="1448" height="709" alt="image" src="https://github.com/user-attachments/assets/3f870f10-0ed3-4a40-85de-aa3e43463777" /><br>

| 단계                                         | 설명                          |
| ------------------------------------------ | --------------------------- |
| ① MCU가 TRIG 핀을 10 µs 동안 HIGH로 함            | “발사 명령”                     |
| ② HC-SR04 내부에서 40 kHz 초음파를 8주기(≈200 µs) 발사 | 이게 바로 “8 cycle sonic burst” |
| ③ 발사 후 ECHO 핀 LOW → HIGH → LOW             | HIGH 구간 길이가 왕복 시간           |
| ④ MCU가 이 시간을 재서 거리 계산                      | 거리 = 음속 × 시간 ÷ 2            |<br>



### (2) Pinout & Configuration

#### 01) RCC(Reset & Clcok Contorl)
<img width="1105" height="552" alt="image" src="https://github.com/user-attachments/assets/fea9eacc-5b89-47b8-8bb4-f71d4eb84576" /><br>


#### 02) TIM1 설정
- 정밀도가 더 높은 TIM1 사용

=>**LED 깜빡이기** 기능을 구현하려면, 간단한 PWM 출력이 가능한 **TIM3**을 사용하는 것이 적합
- TIM1 타이머에 공급되는 클럭 확인: **64(MHz)**<br>
  
<img width="1146" height="651" alt="image" src="https://github.com/user-attachments/assets/73111b99-5f19-40d1-b716-c83c7d726095" /><br>
- Prescaler와 Counter Period 설정값에 의해 타이머 인터럽트의 발생 주기 결정
  => Prescaler: 해상도(입력 클럭을 나누는 값), prescaler=n 이라면 n개의 클럭이 입력될 때마다 1개의 클럭 출력<br>
  => Counter Period: 카운터가 0부터 카운트할 최댓값<br>
- Prescarler = 63, Counter Period 999<br>
  => Prescarler를 64로 설정: 타이머 공급(64,000,000Hz)/64= 1,000,000 Hz 클럭 공급<br>
                           : 1초에 1,000,000 클럭인가 = 1clk 당, 1/1,000,000초 소요<br>
  => Counter Period를 65535으로 설정: 더 먼 거리 측정을 위해서 최대 값 설정<br>
                                   : 인터럽트 주기 1/1,000,000초 * 65536

  #### 03) 핀 패정
<img width="1732" height="669" alt="image" src="https://github.com/user-attachments/assets/a84c9fd3-fa38-40dd-95e8-5fe933b9ab31" />
<img width="842" height="595" alt="image" src="https://github.com/user-attachments/assets/09dc226c-cd53-4fe7-90cc-f8d973016372" /><br>

- TRIG(**PC7**):D9(IO_13)<br>
- Echo(**PB0**):A3(IO-34)<br>
- TRIG2(**PA8**):D7(IO_15)<br>
- Echo2(**PA4**):A2(IO-35)<br>
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
 /* USER CODE BEGIN 0 */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
  if (ch == '\n')
    HAL_UART_Transmit (&huart2, (uint8_t*) "\r", 1, 0xFFFF);
  HAL_UART_Transmit (&huart2, (uint8_t*) &ch, 1, 0xFFFF);

  return ch;
}

void timer_start(void)
{
   HAL_TIM_Base_Start(&htim1);
}

void delay_us(uint16_t us)
{
   __HAL_TIM_SET_COUNTER(&htim1, 0); // 타이머 카운터 0으로 초기화
   while((__HAL_TIM_GET_COUNTER(&htim1))<us); // 지정된 us까지 대기
}

//////////////////////////////////
void trig(void)
   {
       HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, 1);
       delay_us(10);
       HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, 0);
   }


long unsigned int echo(void)
      {
          long unsigned int echo = 0;

          while(HAL_GPIO_ReadPin(ECHO_GPIO_Port, ECHO_Pin) == 0)
               __HAL_TIM_SET_COUNTER(&htim1, 0);// Echo로 들어오는 신호 없으면 타이머 계속 초기화
               while(HAL_GPIO_ReadPin(ECHO_GPIO_Port, ECHO_Pin) == 1);
               echo = __HAL_TIM_GET_COUNTER(&htim1);// High 지속시간 측정
          if( echo >= 240 && echo <= 23000 ) return echo;// 노이저 필터터
          else return 0;
      }

void trig2(void)
      {
          HAL_GPIO_WritePin(TRIG2_GPIO_Port, TRIG2_Pin, 1);
          delay_us(10);
          HAL_GPIO_WritePin(TRIG2_GPIO_Port, TRIG2_Pin, 0);
      }

long unsigned int echo2(void)
         {
             long unsigned int echo2 = 0;

             while(HAL_GPIO_ReadPin(ECHO2_GPIO_Port, ECHO2_Pin) == 0){}
                  __HAL_TIM_SET_COUNTER(&htim1, 0);
                  while(HAL_GPIO_ReadPin(ECHO2_GPIO_Port, ECHO2_Pin) == 1);
                  echo2 = __HAL_TIM_GET_COUNTER(&htim1);
             if( echo2 >= 240 && echo2 <= 23000 ) return echo2;
             else return 0;
         }
/* USER CODE END 0 */
```
=>void trig(void): TRIG 핀에 10us 동한 High 신호 보내는 함수(HC-SR04는 내부적으로 8사이클(8-cycle)의 40kHz 초음파를 발사) <br>
=>long unsigned int echo(void): 초음파 수신 시간 측정 함수

```c
/* USER CODE BEGIN 2 */
  timer_start();
  printf("===HC-SR04 TEST===\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
       trig();// 첫 번째 초음파 센서에서 10us 펄스 발생생
       int echo_time = echo(); // 첫 번재 센서의 Echo 신호 시간 측정
       HAL_Delay(60);// 센서 간섭 방
       trig2();
       int echo_time2 = echo2();


       if(echo_time!=0 ||echo_time2 != 0){
           int dist = (int)(17 * echo_time / 100);
           int dist2 = (int)(17 * echo_time2 / 100);
           printf("DistanceL = %d(mm) / DistanceR = %d(mm)\n", dist, dist2);
       }

       else printf("Out of Range!\n");

    /* USER CODE END WHILE */

```
=> int dist = (int)(17 * echo_time / 100): 거리(mm)= 시간(us) * 0.34(mm/us)/2 = 17*시간/100<br>
(음속 = 0.34 mm/µs (340 m/s = 340,000 mm/s → 0.34 mm/µs))
