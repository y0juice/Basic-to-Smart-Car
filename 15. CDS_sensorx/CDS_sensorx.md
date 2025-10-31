# 15. CDS_sensorx
**기능) 아두이노 보드에 내장된 조도센서(CDS)를 활용하여 ADC->전압-> 밝기로 변환여 Tera Teram으로 출력<br> 
<img width="1056" height="652" alt="image" src="https://github.com/user-attachments/assets/5d52731e-2903-4cf8-ae94-0bdcbcccfe34" /><br>
<img width="1102" height="610" alt="image" src="https://github.com/user-attachments/assets/e7002b2e-608f-4fe0-bd3d-f69cadee2900" /><br>


__
### (1) 이론 설명 ###
- STM32 ADC 해상도: 12bit → 0~4095<br>
- 기준 전압: 3.3V<br>
=> 4095일 때 3.3V, 0일 때 0V<br>
| ADC 값 | 전압(V) | 밝기 상태 |
| ----- | ----- | ----- |
| 100   | 0.08V | 어두움   |
| 2000  | 1.6V  | 중간    |
| 3500  | 2.8V  | 매우 밝음 |<br>

### (2) Pinout & Configuration

#### 01) RCC(Reset & Clcok Contorl)
<img width="1105" height="552" alt="image" src="https://github.com/user-attachments/assets/fea9eacc-5b89-47b8-8bb4-f71d4eb84576" />


#### 02)ADC 설정: ADC2사용
<img width="2538" height="825" alt="image" src="https://github.com/user-attachments/assets/8d274236-b78a-428e-b774-e953acb69734" /><br>
=> ADC 클러기 PCLK2/6 =106MHZ : 안정적인 ADC 샘플링 속도를 확보하기 위해 <br>
=> ADC - IN10 활성화 하면 PC0(ADC1_IN10):(ADC1_IN10) 자동으로 핀 배정<br>

#### 03)float 출력 설정
<img width="814" height="525" alt="image" src="https://github.com/user-attachments/assets/a827744c-44c5-4b12-a6f2-de9e23ec5cce" /><br>
-uhuart2(Teraterm)으로  float를 출력할 때, 제대로 출력될 수 있도록<br>

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
```
```c
/* USER CODE BEGIN 1 */
	uint32_t adc_value = 0;
	float voltage = 0.0f;
	uint32_t counter = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  printf("STM32F103 CDS Sensor Reading Example\r\n");
  printf("System Clock: 64MHz\r\n");
  printf("ADC Channel: PC0\r\n");
  printf("Starting measurements...\r\n\r\n");

  /* Calibrate ADC */
  HAL_ADCEx_Calibration_Start(&hadc1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	    /* Start ADC conversion */
		    HAL_ADC_Start(&hadc1);

		    /* Wait for conversion to complete */
		    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
		    {
		      /* Get ADC value */
		      adc_value = HAL_ADC_GetValue(&hadc1);

		      /* Convert ADC value to voltage (3.3V reference, 12-bit ADC) */
		      voltage = (adc_value * 3.3f) / 4095.0f;

		      /* Print results */
		      printf("[%lu] CDS ADC Value: %lu, Voltage: %.3fV\r\n",
		             counter++, adc_value, voltage);

		      /* CDS 센서 값에 따른 조도 상태 출력 */
		      if (voltage < 0.5f)
		      {
		        printf("       Light Level: Very Dark\r\n");
		      }
		      else if (voltage < 1.0f)
		      {
		        printf("       Light Level: Dark\r\n");
		      }
		      else if (voltage < 2.0f)
		      {
		        printf("       Light Level: Medium\r\n");
		      }
		      else if (voltage < 2.5f)
		      {
		        printf("       Light Level: Bright\r\n");
		      }
		      else
		      {
		        printf("       Light Level: Very Bright\r\n");
		      }
		      printf("\r\n");
		    }

		    /* Stop ADC */
		    HAL_ADC_Stop(&hadc1);

		    /* Wait 500ms before next reading */
		    HAL_Delay(500);
		  }
```
=>
| 표기     | 자료형      | 의미              | 메모리 크기 |
| ------ | -------- | --------------- | ------ |
| `0`    | `int`    | 정수 0            | 4byte  |
| `0.0`  | `double` | 실수 0 (기본 부동소수점) | 8byte  |
| `0.0f` | `float`  | 실수 0 (단정밀도)     | 4byte  |<br>

=> if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) : 프로그램 무한 대기 상태 방지<br>
- ADC변환 약 수 us: 100ms 동안 기다리고 완료(1), 100ms 초과(HAL_TIMEOUT)<br>



