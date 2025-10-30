# 13. I2C-EEPROM
**기능) Free RTOS 환경으로 project 생성**<br>
<img width="993" height="583" alt="image" src="https://github.com/user-attachments/assets/23a9b0dd-4ad7-48e7-bfd8-e0cfb91a298d" />
=> PIN:VCC/SCL/SDA/GND<br>

__
### (1) 이론 설명 ###
<img width="908" height="372" alt="image" src="https://github.com/user-attachments/assets/e106ec1a-2121-4bfd-bcfd-a44b2dd8cf74" /><br>
=> I2C: **2개의 선(SCL, SDA)** 으로 여러 장치를 연결하는 직렬 통신 방식<br>
=> EEPROM: 비휘발성 메모리<br>
=> STM32(Master) & EEPROM(Slave)<br>
=> 용도: 설정값, 시리얼번호, 보정데이터, 로그, 센서보정값 등 저장<br>

### (2) Pinout & Configuration

#### 01) RCC(Reset & Clcok Contorl)
<img width="1105" height="552" alt="image" src="https://github.com/user-attachments/assets/fea9eacc-5b89-47b8-8bb4-f71d4eb84576" />


#### 02)I2C 설정: I2C1
: Middie and Software Packs<br>
<img width="1970" height="828" alt="image" src="https://github.com/user-attachments/assets/7b2719c6-76e2-40ff-9a74-18f3dd4528b0" />
| 구분         | **V1**                      | **V2**                         |
| ---------- | --------------------------- | ------------------------------ |
| **시대**     | 예전 버전 (기본 RTOS용)            | 최신 버전 (TrustZone·멀티코어 대응)      |
| **API 형태** | `osThreadCreate()` 같은 단순 함수 | `osThreadNew()` 식으로 바뀜, 구조체 기반 |
| **메모리 관리** | 주로 정적(static) 생성            | 동적(dynamic) 객체 생성 지원           |
=> Task가 총 3개이므로 사실 Task02까지만 만들면 돼요!!<br>

  #### 03) 핀 패정
<img width="554" height="515" alt="image" src="https://github.com/user-attachments/assets/9d68606b-610f-45a5-8bf0-8cfab4921cf2" /><br>
- LED로는 LD2, 버튼으로는 B1 사용 <br>


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
  void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
	  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	  osDelay(100);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void *argument)
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
  for(;;)
  {
      printf("park yun ju \n");
	  osDelay(1000);
  }
  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*
*/

/* USER CODE END Header_StartTask04 */
void StartTask04(void *argument)
{
  /* USER CODE BEGIN StartTask04 */
  /* Infinite loop */
//    static uint8_t ledOn = 0;       // LED 상태 저장
//    static GPIO_PinState lastButtonState = GPIO_PIN_RESET;  // 이전 버튼 상태 저장
//
//    for(;;)
//    {
//        GPIO_PinState buttonState = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);
//
//        // 버튼이 눌렸다가 떼어졌을 때만 토글
//        if(buttonState == GPIO_PIN_SET && lastButtonState == GPIO_PIN_RESET)
//        {
//            ledOn = !ledOn;  // 상태 토글
//            HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, (ledOn ? GPIO_PIN_SET : GPIO_PIN_RESET));
//        }
//
//        lastButtonState = buttonState;  // 현재 상태를 다음 반복용으로 저장
//        osDelay(50); // 디바운스 및 CPU 부담 완화

    static GPIO_PinState lastButtonState = GPIO_PIN_SET;

    for(;;)
    {
        GPIO_PinState buttonState = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);

        // 버튼이 눌렸다가 떼어졌을 때만 토글
        if(buttonState == 1 && lastButtonState == 0)
        {
            // 현재 LED 상태 읽어서 반대로 바꿈
            if(HAL_GPIO_ReadPin(LED2_GPIO_Port, LED2_Pin) == GPIO_PIN_SET)
                HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0); // 끔
            else
                HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);   // 켬
        }

        // 현재 버튼 상태 저장
        lastButtonState = buttonState;
    }

  }
	  /* USER CODE END 3 */
```
=>StartTask02: 0.1초마다 LED toggle<br>
=>StartTask03:  1초마다 '박윤주' print<br>
=>StartTask04: 버튼 누를 때마다 LED toggle<br>
=>**osDelay()**: FreeRTOS에서는 HAL_Delay() 대신 사용<br>
