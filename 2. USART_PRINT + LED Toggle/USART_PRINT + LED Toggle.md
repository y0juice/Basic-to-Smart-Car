# 2) USART_PRINT + LED Toggle
기능1) Tera term을 통해 입력받은 키 한줄로 출력<br>
기능2) B1으로 LED Toggle<br>
기능3) a 입력시: LED on/ 그외 다른 키: LED off<br>
<img width="415" height="345" alt="image" src="https://github.com/user-attachments/assets/1df061dd-86ba-4a18-a5a7-256feece9460" />
![2  USART_PRINT LED Toggle2 (2)](https://github.com/user-attachments/assets/bc87972d-5d15-43d1-b167-345993d5e45d)



### (1) 터미널 통신 프로그램 설치
- Teraterm download link: [https://github.com/TeraTermProject/teraterm/releases?authuser=0]
  <img width="913" height="335" alt="image" src="https://github.com/user-attachments/assets/531bfc1c-fa3f-4a66-9a6e-7d6bbc4f883b" /><br>
  
### (2) Pinout & Configuration
#### 01) RCC(Reset & Clcok Contorl)
<img width="1105" height="552" alt="image" src="https://github.com/user-attachments/assets/fea9eacc-5b89-47b8-8bb4-f71d4eb84576" />

#### 02) USART2(Universal Synchronous ASynchronous RecieverReceiver/Transmitter) 설정
- 총 3개 존재: USART1(블루투스, GPS, 디버그), USART2(PC통신: USB), USART3(다른 MCU, 모듈)
***USB 포트를 사용: USART2***
  <img width="1870" height="640" alt="image" src="https://github.com/user-attachments/assets/c9366d71-9230-49b9-9ba2-34b3a17afe1b" /><br?
  
### (3) 코드작성
- main.c 추가 코드
```c
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */
```
=> 입출력 관련 함수 선언
___
```c
/* USER CODE BEGIN 0 */

//버튼(외부 인터럽트)-> 콜백함수 실행 => 핀번호 확인(B1_Pin(13) -> LED 토글(누르면 켜지다가 다음번누를때 꺼짐)
void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
	case B1_Pin://B1을 눌렀을 때
		HAL_GPIO_TogglePin (LD2_GPIO_Port, LD2_Pin);// LED가 켜져잇으면 끄고, 켜져 있으면 견다: Toggle
		break;

	default:// 그 외 버튼은 무
		;
	}
}
```
=>B1으로 LED Toggle
```c
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
   if (ch == '\n')
      HAL_UART_Transmit (&huart2, (uint8_t*) "\r", 1, 0xFFFF);
   HAL_UART_Transmit (&huart2, (uint8_t*) &ch, 1, 0xFFFF);//UART2를 사용해서 1글자씩 전송, 0xFFFF: 전송 대기 최대 시간 = HAL_MAX_DELAY

   return ch;
} 
/* USER CODE END 0 */

```
=> printf("Hello\n"); 실제로 PC에서는 UART로 “Hello\r\n”
___

```c
/* USER CODE BEGIN 2 */
  uint8_t ch;//uint8_t: 0-255 범위의 정수 1byte: 문자 1개를 저장하는 용도

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	 HAL_UART_Receive(&huart2, &ch, 1, HAL_MAX_DELAY);//HAL_MAX_DELAY: 문자 들어올때까지 무한 대기
     printf("%c", ch);
     fflush(stdout);//한줄에 쓰기. flush로 즉시 출력
     HAL_Delay(100);

	 if(ch=='a')
		 HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);// 입력이 a면 LD2 on

	 else
		 HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);// 입력이 a가아니면 LD2 off
     HAL_Delay(100);

     /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
```


  
