# 13. JoyStick
<img width="1243" height="688" alt="image" src="https://github.com/user-attachments/assets/7c7ba32f-6942-455c-b76b-ad552385c18f" /><br>

**기능) 조이스틱(2축) 값을 ADC로 읽고, DMA + 타이머 + UART 를 이용해 실시간으로 필터링 후 퍼센트 값으로 출력하는 시스템** <br>
<img width="200" height="200" alt="image" src="https://github.com/user-attachments/assets/185d2b4e-7991-44f3-b6ed-f571c8dce63b" /><br>
=> PIN:GND/+5V/VRX/VRY/SW<br>
=>
```
┌────────────┐     ┌────────────┐     ┌─────────────┐      ┌──────────────────────────┐
│ Joystick   │ --> │ ADC (2채널)│ --> │ DMA (ADC→RAM)│ --> │ process_joystick_data()  │-->UART (printf)
└────────────┘     └────────────┘     └─────────────┘      └──────────────────────────┘
```
- 조이스틱 X, Y축 → PA0, PA1 (ADC 채널 0, 1)
- ADC : 아날로그 전압 (0~3.3V) → 12비트 디지털 값(0~4095)
- DMA : CPU 도움 없이 ADC 데이터를 adc_buffer[] 로 자동 저장
- TIM2 : 50ms마다 인터럽트 → process_joystick_data() 호출
- UART2 : 결과를 터미널(TeraTerm 등)로 출력
                                                         


__
### (1) 이론 설명 ###
=> 조이스틱의 두축(Analog) 신호를 STM32가 Digital로 변환해서 읽음: **ADC필요**


### (2) Pinout & Configuration

#### 01) RCC(Reset & Clcok Contorl)
<img width="1105" height="552" alt="image" src="https://github.com/user-attachments/assets/fea9eacc-5b89-47b8-8bb4-f71d4eb84576" />


#### 02)ADC 설정: ADC1(+ Clock Configuration 설정)
<img width="708" height="557" alt="image" src="https://github.com/user-attachments/assets/6e904a0c-efcd-44e9-8748-ea08ad9b1337" /><br>
=> Number of Conversion을 먼저 2로 바꿔야지 다른 설정들 할 수 있음 <br>
=>IN0-> PA0 활성화/ IN1-> PA1 활성화<br>
<img width="1200" height="669" alt="image" src="https://github.com/user-attachments/assets/867bd3c3-fc50-4266-8312-7dee43925918" />


#### 03) DMA 설정
<img width="962" height="550" alt="image" src="https://github.com/user-attachments/assets/df04119e-d3d4-498f-8b37-c6f22c02938e" /><br>
=>DMA (Direct Memory Access): CPU 대신 주변장치가 메모리에 직접 데이터를 옮기게 하는 기능<br>
- 즉, CPU는 다른 일(통신, 제어, 계산 등)을 하면서도 ADC 값은 자동으로 RAM에 쌓임<br>
- Mode: Circular ( 자동 반복 되도록)<br>
```
┌──────────┐       ┌──────────────┐       ┌────────────┐
│  Sensor  │ ───► │   ADC (12bit) │ ───► │ DMA (16bit)│ ───► RAM 배열에 저장
└──────────┘       └──────────────┘       └────────────┘
```
#### 03) TIM 설정: TIM2(+ NVIC 인터럽트설정)
<img width="897" height="511" alt="image" src="https://github.com/user-attachments/assets/9ff7552a-b1ab-449f-a137-6a0c950cfeae" /><br>
=> Prescaler: 6399 ( timer clk= 64000000/6400=10000 Hz)-> 1cnt 당 100us <br>
=> Counter Period(ARR): 499(500/10000=0.05초) -> 타이머 주기 50ms<br>
(500카운트마다 인터럽트 발생)<br>
<img width="1260" height="683" alt="image" src="https://github.com/user-attachments/assets/15d6ca79-6d78-449f-89fa-aa31360e4e4c" /><br>



___
#### Generate Code ####
___
  #### (3) 코드작성
  -main.c 추가 코드
```c
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */
```
```c
/* USER CODE BEGIN PD */
#define ADC_BUFFER_SIZE 2//(x축,y축 총 2개)
#define FILTER_SIZE 8        // 이동평균 필터 크기
#define ADC_MAX_VALUE 4095   // 12bit ADC 최대값
/* USER CODE END PD */
```
/* USER CODE BEGIN PV */
uint16_t adc_buffer[ADC_BUFFER_SIZE];  // DMA 버퍼
uint16_t joystick_x_raw = 0;           // 조이스틱 X축 원시값
uint16_t joystick_y_raw = 0;           // 조이스틱 Y축 원시값

// 이동평균 필터를 위한 배열
uint32_t x_filter_buffer[FILTER_SIZE] = {0}; 
uint32_t y_filter_buffer[FILTER_SIZE] = {0};
uint8_t filter_index = 0;

// 필터링된 값
uint16_t joystick_x_filtered = 0;
uint16_t joystick_y_filtered = 0;

// 백분율로 변환된 값 (-100 ~ +100)
int16_t joystick_x_percent = 0;
int16_t joystick_y_percent = 0;

char uart_buffer[100];  // 필요시 사용할 버퍼 (현재는 printf 사용)
/* USER CODE END PV */

```c
/* USER CODE BEGIN PFP */
void process_joystick_data(void);
uint16_t apply_moving_average_filter(uint16_t new_value, uint32_t *filter_buffer);
int16_t convert_to_percentage(uint16_t adc_value);
/* USER CODE END PFP */
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
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  if (ch == '\n')
    HAL_UART_Transmit(&huart2, (uint8_t*)"\r", 1, 0xFFFF);
  HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, 0xFFFF);

  return ch;
}

uint16_t apply_moving_average_filter(uint16_t new_value, uint32_t *filter_buffer)
{
    static uint8_t x_init = 0, y_init = 0;
    uint32_t sum = 0;

    // 필터 버퍼 구분 (X축 또는 Y축)
    if (filter_buffer == x_filter_buffer) {
        if (!x_init) {
            // 초기화: 모든 버퍼를 첫 번째 값으로 채움
            for (int i = 0; i < FILTER_SIZE; i++) {
                filter_buffer[i] = new_value;
            }
            x_init = 1;
            return new_value;
        }
    } else {
        if (!y_init) {
            // 초기화: 모든 버퍼를 첫 번째 값으로 채움
            for (int i = 0; i < FILTER_SIZE; i++) {
                filter_buffer[i] = new_value;
            }
            y_init = 1;
            return new_value;
        }
    }

    // 새로운 값을 버퍼에 추가
    filter_buffer[filter_index] = new_value;

    // 평균 계산
    for (int i = 0; i < FILTER_SIZE; i++) {
        sum += filter_buffer[i];
    }

    return (uint16_t)(sum / FILTER_SIZE);
}

/**
  * @brief  ADC 값을 백분율로 변환 (-100 ~ +100)
  * @param  adc_value: ADC 값 (0 ~ 4095)
  * @retval 백분율 값
  */
int16_t convert_to_percentage(uint16_t adc_value)
{
    // ADC 중앙값을 기준으로 -100 ~ +100으로 변환
    int16_t centered_value = (int16_t)adc_value - (ADC_MAX_VALUE / 2);
    int16_t percentage = (centered_value * 100) / (ADC_MAX_VALUE / 2);

    // 범위 제한
    if (percentage > 100) percentage = 100;
    if (percentage < -100) percentage = -100;

    return percentage;
}

/**
  * @brief  조이스틱 데이터 처리
  * @param  None
  * @retval None
  */
void process_joystick_data(void)
{
    // 원시 ADC 값 읽기
    joystick_x_raw = adc_buffer[0];  // ADC Channel 0 (PA0)
    joystick_y_raw = adc_buffer[1];  // ADC Channel 1 (PA1)

    // 이동평균 필터 적용
    joystick_x_filtered = apply_moving_average_filter(joystick_x_raw, x_filter_buffer);
    joystick_y_filtered = apply_moving_average_filter(joystick_y_raw, y_filter_buffer);

    // 필터 인덱스 업데이트 (두 축 공통 사용)
    filter_index = (filter_index + 1) % FILTER_SIZE;

    // 백분율로 변환
    joystick_x_percent = convert_to_percentage(joystick_x_filtered);
    joystick_y_percent = convert_to_percentage(joystick_y_filtered);
}

/**
  * @brief  타이머 콜백 함수 (주기적 ADC 읽기용)
  * @param  htim: 타이머 핸들
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2) {
        // 조이스틱 데이터 처리
        process_joystick_data();

        // UART로 데이터 출력 (디버깅용)
        printf("X: %d%% (%d), Y: %d%% (%d)\n",
                joystick_x_percent, joystick_x_filtered,
                joystick_y_percent, joystick_y_filtered);
    }
}
/* USER CODE END 0 */
```
=> apply_movine_averge_filter(): 노이즈를 제거해서 조이스틱으로 좀더 부드럽게 조작할 수 있도록<br>
```c
/* USER CODE BEGIN 2 */
  if (HAL_DMA_Init(&hdma_adc1) != HAL_OK) {
       Error_Handler();
   }

   // ADC1 핸들과 DMA 링크
   __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);

   // ADC 캘리브레이션
   HAL_ADCEx_Calibration_Start(&hadc1);

   // DMA를 사용한 연속 ADC 변환 시작
   HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_BUFFER_SIZE);

   // 타이머 시작 (50ms 주기로 데이터 처리)
   HAL_TIM_Base_Start_IT(&htim2);

   // 시작 메시지
   printf("STM32F103 조이스틱 ADC 읽기 시작\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
   while (1){
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
   HAL_Delay(10);  // 메인 루프 딜레이
   }

  /* USER CODE END 3 */
```
