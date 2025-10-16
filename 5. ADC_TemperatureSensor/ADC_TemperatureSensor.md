# 5. ADC_TemperatureSensor

**기능) STM32F103RB ADC1의 Temerature Sensor 채널에 연결된 내부의 온도센서를 이용하여 온도를 측정 -> Teraterm** <br>
<img width="812" height="482" alt="image" src="https://github.com/user-attachments/assets/4360706c-5e36-4e74-9f45-6338fac0da49" /><br>
=> ADC 온도랑 섭씨 온도랑 반비례



### (1) Pinout & Configuration

#### 01) RCC(Reset & Clcok Contorl)
<img width="1105" height="552" alt="image" src="https://github.com/user-attachments/assets/fea9eacc-5b89-47b8-8bb4-f71d4eb84576" /><br>

#### 02) USART2(Universal Synchronous ASynchronous RecieverReceiver/Transmitter) 설정
- 총 3개 존재: USART1(블루투스, GPS, 디버그), USART2(PC통신: USB), USART3(다른 MCU, 모듈)
***USB 포트를 사용: USART2***: TeraTerm으로 데이터를 출력하기 위해 printf()를 사용
  <img width="1870" height="640" alt="image" src="https://github.com/user-attachments/assets/c9366d71-9230-49b9-9ba2-34b3a17afe1b" /><br>

#### 03) ADC1 설정
<img width="1634" height="647" alt="image" src="https://github.com/user-attachments/assets/28d82642-86d3-4c11-95c2-858932a7e1e4" /><br>
=> ADC1 단일 채널, **계속 변환**, 오른쪽 정렬, 단순 온도 측정용<br>
=> Sampling Time = 13.5cycles → 전체 변환 시간 = 13.5 +12.5 = 26 ADC clock cycles <br>
  : 길면 정확도 ↑ & 변환 속도 ↓, 짧으면  길면 정확도 ↓,변환 속도 ↑
- clk 조절: ADC 클럭은 최대 8~12 MHz 권장(64M/8= 8MHz로 안정)<br>
<img width="2339" height="601" alt="image" src="https://github.com/user-attachments/assets/0dca8d3e-e40f-4a3b-a1b3-f881c7ada9db" />
#### 04) printf()에서 %f 실수 출력을 위한 Cube IDE 설정 변경

<img width="1105" height="705" alt="image" src="https://github.com/user-attachments/assets/e674a280-8aa5-442a-b8a2-4dcf12609905" />



___
#### Generate Code ####
___

  #### 03) 코드작성
  -main.c 추가 코드
  ```c
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */
```

```c
/* USER CODE BEGIN PV */
const float AVG_SLOPE = 4.3E-03;//1도당 전압 변화량 = 0.0043ㅍ
const float V25 = 1.43;
const float ADC_TO_VOLT= 3.3/4096;
/* USER CODE END PV */
```
=> AVG_SLOPE(전압 변화률): 온도가 1 °C 변하면 센서 전압이 4.3 mV 변함.<br>
=> V25 = 1.43;// 온도 25 °C에서 내부 센서 전압 = 1.43V(datasheet에 명시)<br>
=> ADC_TO_VOLT(실제 전압 변환 계수): 12bit ADC(0~4095), ADC 0 이면 0V/ ADC 4095면 보통 3.3V<br>
| ADC 값 (12bit) | 전압 V_sense (V) | 온도 T (°C) |
| ------------- | -------------- | --------- |
| 4095          | 3.3            | 매우 낮음     |
| 3000          | 2.42           | 낮음        |
| 2000          | 1.61           | 약 25      |
| 1000          | 0.805          | 높음        |
| 0             | 0              | 매우 높음     |<br>



```c
/* USER CODE BEGIN 0 */
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
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
/* USER CODE END 0 */
```
=> printf()를 UART로 전달해서 시리얼 모니터에서 출력
```c
  /* USER CODE BEGIN 2 */
  if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)
  {
	  Error_Handler();
  }

  if (HAL_ADC_Start (&hadc1) != HAL_OK)
  {
	  Error_Handler();
  }

  uint16_t adc1;//디지털 값 변수

  float vSense;// 온도 전압 값 변수
  float temp;// 섭씨 온도 값 변
  /* USER CODE END 2 */
```
=> (HAL_ADCEx_Calibration_Start(&hadc1): ADC 보정 함수<br>
=> (HAL_ADC_Start (&hadc1) != HAL_OK): ADC 시작 함수<br>
```c
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {

    /* USER CODE BEGIN 3 */
	  HAL_ADC_PollForConversion (&hadc1, 100);
	  adc1 = HAL_ADC_GetValue (&hadc1);// ADC 값 저
	// printf("ADC1_temperature: %d \n", adc1);
    /* USER CODE END WHILE */
	  vSense = adc1 * ADC_TO_VOLT;
	  temp = (V25-vSense) / AVG_SLOPE +25.0;
	  printf ("temperature: %d, %f \n", adc1, temp);

	  HAL_Delay(100);

  }
  /* USER CODE END 3 */
}
```
=> HAL_ADC_PollForConversion(&hadc1, 100): ADC 변환이 끝날 때까지 기다리는 함수 (최대 100ms까지 기다림)<br>
=>  temp = (V25-vSense) / AVG_SLOPE +25.0: 25°C 기준 전압 V25보다 얼마나 낮거나 높은지 계산한 후, 기준점인 25도 더하기




