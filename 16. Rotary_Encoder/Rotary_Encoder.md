# 16. Rotary_Encoder

**기능) 로터리 인코더를 읽고,회전 방향(시계/반시계)과 카운트 값을 UART 터미널로 실시간 출력.** <br>
<img width="200" height="200" alt="image" src="https://github.com/user-attachments/assets/21d99bce-1bf4-411d-a200-ab56f945cc14" /><br>
=> PIN: CLK/DT/SW/+(VDD)/GND<br>
=> 실제로 좀 불안정하게 값이 흔들림 <br>
=>A상(CLK), B상(DT):  90도 위상차<br>
```
A B(시계방향)
0 0  →  0 1  →  1 1  →  1 0  →  (다시 0 0)
```
```
A B(반시계 방향)
0 0  →  1 0  →  1 1  →  0 1  →  (다시 0 0)
```

__
### (1) 이론 설명 ###
<img width="908" height="372" alt="image" src="https://github.com/user-attachments/assets/e106ec1a-2121-4bfd-bcfd-a44b2dd8cf74" /><br>
=> I2C: **2개의 선(SCL, SDA)** 으로 여러 장치를 연결하는 직렬 통신 방식<br>
=> EEPROM: 비휘발성 메모리<br>
-주소 범위: 0x0000 ~ 0x7FFF (총 32,768 Byte = 32KB)<br>
=> STM32(Master) & EEPROM(Slave)<br>
=> 용도: 설정값, 시리얼번호, 보정데이터, 로그, 센서보정값 등 저장<br>

### (2) Pinout & Configuration

#### 01) RCC(Reset & Clcok Contorl)
<img width="1105" height="552" alt="image" src="https://github.com/user-attachments/assets/fea9eacc-5b89-47b8-8bb4-f71d4eb84576" />


#### 02) Pin 설정 
<img width="1179" height="611" alt="image" src="https://github.com/user-attachments/assets/6f10caeb-39e5-4360-a777-9e37294a4c99" /><br>

=> Encoder PIN: CLK/DT/SW/+(signal)/GND<br>
- PA0(GPIO_EXTI1):CLK_Pin / PA1(GPIO_EXTI1):DT_Pin
=> LED: 자동 설정<br>
- PA5(GPIO_Output): LD2
=> 버튼: 자동설정
- PC13(GPIO_EXTI13):B1_Pin

#### 04) NCIV 설정 (인터럽트 우선순위)
<img width="926" height="758" alt="image" src="https://github.com/user-attachments/assets/4f43b37d-4861-4faf-ad4e-e3c560e319e4" /><br>
=> Priority<br>
- 인코더 인터럽트(0,1): 높은 우선순위 (1)<br>
- 버튼 인터럽트(13): 낮은 우선순위 (2)<br>
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
// 인코더 관련 변수 (그레이 코드 방식)
volatile int32_t encoder_count = 0;
volatile uint8_t encoder_last_encoded = 0;

// 디바운스를 위한 변수
volatile uint32_t last_interrupt_time = 0;
#define DEBOUNCE_DELAY 5  // ms 단위
/* USER CODE END PV */
```
```c

```c
/* USER CODE BEGIN 0 */

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  if (ch == '\n')
    HAL_UART_Transmit(&huart2, (uint8_t*)"\r", 1, 0xFFFF);
  HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, 0xFFFF);

  return ch;
}

 * @brief 로터리 인코더 초기화 (그레이 코드 방식)
 */
void Encoder_Init(void)
{
  // 초기 상태 읽기
  uint8_t clk = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
  uint8_t dt = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
  encoder_last_encoded = (clk << 1) | dt;// clk 와 DT의 현재 상태를 합친것
  encoder_count = 0;
}

/**
 * @brief GPIO 외부 인터럽트 콜백 함수 (그레이 코드 방식)
 * 안정적인 로터리 인코더 감지
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  uint32_t current_time = HAL_GetTick();

  // 디바운스 처리
  if (current_time - last_interrupt_time < DEBOUNCE_DELAY) {
    return;
  }
  last_interrupt_time = current_time;

  // 인코더 핀들 처리 (PA0, PA1)
  if (GPIO_Pin == GPIO_PIN_0 || GPIO_Pin == GPIO_PIN_1)
  {
    // 현재 상태 읽기
    uint8_t clk = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
    uint8_t dt = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
    uint8_t encoded = (clk << 1) | dt;  // 2비트 상태 생성

    uint8_t sum = (encoder_last_encoded << 2) | encoded;  // 이전 상태와 현재 상태 결합

    // 회전 방향 결정 (그레이 코드 기반)
    if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
      encoder_count++;  // 시계방향
    }
    if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
      encoder_count--;  // 반시계방향
    }

    encoder_last_encoded = encoded;
  }
}
/* USER CODE END 0 */
```
```c
/* USER CODE BEGIN 2 */
  // 로터리 인코더 초기화
  	Encoder_Init();

  	// 타이머 시작 (시간 측정용)
  	HAL_TIM_Base_Start(&htim2);

  	printf("=============================\r\n");
  	printf("STM32F103 로터리 인코더 테스트 시작\r\n");
  	printf("시스템 클록: %lu Hz\r\n", SystemCoreClock);
  	printf("그레이 코드 기반 안정적 감지 방식 적용\r\n");
  	printf("인코더 카운트 변화를 모니터링합니다...\r\n");
  	printf("=============================\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  // 인코더 값 변화 감지 및 출력
	 	    if (encoder_count != last_encoder_count) {
	 	      printf("인코더 카운트: %ld", encoder_count);

	 	      if (encoder_count > last_encoder_count) {
	 	        printf(" (시계방향)\r\n");
	 	      } else {
	 	        printf(" (반시계방향)\r\n");
	 	      }

	 	      last_encoder_count = encoder_count;
	 	    }

	 	    HAL_Delay(1);  // CPU 부하 감소
    /* USER CODE BEGIN 3 */
```
=> sum = (encoder_last_encoded << 2) | encoded;: [이전A, 이전B, 현재A, 현재B]<br>
- <시계 sum 나올 수 있는 경우의 수><br>

| 이전 (A,B) | 현재 (A,B) | sum (4bit)    | 의미   |
| -------- | -------- | ------------- | ---- |
| 00       | 01       | 0001 (0b0001) | 반시계용 |
| 01       | 11       | 0111 (0b0111) | 반시계용 |
| 11       | 10       | 1110 (0b1110) | 반시계용 |
| 10       | 00       | 1000 (0b1000) | 반시계용 |<br>

- if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)<br>
    encoder_count--;<br>
- <반시계 sum 나올 수 있는 경우의 수><br>

| 이전 (A,B) | 현재 (A,B) | sum (4bit)    | 의미  |
| -------- | -------- | ------------- | --- |
| 00       | 10       | 0010 (0b0010) | 시계용 |
| 10       | 11       | 1011 (0b1011) | 시계용 |
| 11       | 01       | 1101 (0b1101) | 시계용 |
| 01       | 00       | 0100 (0b0100) | 시계용 |<br>

-if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)<br>
    encoder_count++;<br>







