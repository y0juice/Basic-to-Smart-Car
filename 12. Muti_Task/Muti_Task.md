# 12. Muti_Task
**기능) interrut로 project 생성**<br>
Task1: 차이머 인터럽트로 100ms 주기 LED on/off<br>
Task2: main 루프에서 "박윤주"를 teraterm으로 출력<br>
Task3: 버튼으로 LED on/off<br>

__
### (1) 이론 설명 ###
| 구분        | **멀티태스킹(Multi-tasking)**      | **RTOS (Real-Time OS)**          |
| --------- | ----------------------------- | -------------------------------- |
| **정의**    | 여러 작업을 동시에 수행하는 개념            | 여러 작업을 **정확한 시점과 우선순위로 제어**하는 OS |
| **핵심 목적** | 병렬적 처리 (효율)                   | **정확한 타이밍**과 **예측 가능한 응답**       |
| **관리 주체** | 개발자가 직접 task 전환/딜레이 작성        | RTOS가 자동으로 스케줄 관리                |
| **시간 제어** | 단순 딜레이(HAL_Delay 등) 기반        | tick 단위의 정밀 타이머로 제어              |
| **우선순위**  | 없음 (코드 순서대로 실행)               | 각 task마다 priority 존재             |
| **예시**    | `while(1)` 안에서 if문으로 여러 작업 처리 | FreeRTOS, CMSIS-RTOS, RTX 등      |<br>
=>FreeRTOS 는 SysTick으로 스케줄링 vs HAL 라이브러이의 HAL_Delay() SysTick 타이머사용<br>
: 충동 가능성 O<br>
<img width="918" height="340" alt="image" src="https://github.com/user-attachments/assets/7ac49c6e-54a1-423a-96f1-07fd12403c6d" /><br>
:HAL은 TIM6 같은 별도 타이머로 HAL_Delay()를 처리 → 둘이 간섭하지 않게 분리<br>

### (2) Pinout & Configuration

#### 01) RCC(Reset & Clcok Contorl)
<img width="1105" height="552" alt="image" src="https://github.com/user-attachments/assets/fea9eacc-5b89-47b8-8bb4-f71d4eb84576" />


#### 02)TIM: TIM2 사용
<img width="890" height="767" alt="image" src="https://github.com/user-attachments/assets/7e37f9fb-6f17-4871-9965-85449ee3fcfd" /><br>
=> Prescaler=6399/ Period=999 : 64M(system clk)/6400/1000=10Hz<br>

#### 03) NVIC: 버튼 & TIM2 인터럽트 활성화+ Pin 배정
<img width="2038" height="876" alt="image" src="https://github.com/user-attachments/assets/060e4153-ad06-4776-ae6e-97fcd1179cc0" /><br>
=>PA0: LED2(GPIO_Output), PA4:B3(GPIO_EXIT4)-아두이노 보드 사용 <br>

=> LED로는 LD2, 버튼으로는 B1 사용 <br>


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
/* USER CODE BEGIN PV */
volatile int gTimerCnt;
volatile int printflag=0;

/* USER CODE END PV */

```
=> interrupt 변수 선언 **volatile** 사용!
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

/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
//      HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
      gTimerCnt++;  // 100ms 마다 1씩 증가
            if (gTimerCnt >= 10)  // 100ms 인터럽트 기준 10번 = 1초
            {
                gTimerCnt = 0;
                printflag = 1;
            }

}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
		case B1_Pin:
			HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
			break;
		default:
			;
	}
}

/* USER CODE END 0 */
```
=> void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) : 100ms마다 인터럽트 걸려 LED깜빡거림(타이머 설정: 10Hz)<br>

```ㅊ
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      if (printflag == 1)  // 100ms 인터럽트 기준 10번 = 1초
      {
          printf("park yun ju \n");
          printflag = 0;
      }
	  }

```

