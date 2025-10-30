# 13. I2C-EEPROM
**기능) STM32 보드가 I²C 통신(100 kHz) 으로 외부 EEPROM(K24C256) 을 스캔,
저장 → 읽기 → 검증 → UART 출력** <br>
<img width="993" height="583" alt="image" src="https://github.com/user-attachments/assets/23a9b0dd-4ad7-48e7-bfd8-e0cfb91a298d" />
=> PIN:VCC/SCL/SDA/GND<br>

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


#### 02)I2C 설정: I2C1 + 핀 설정
<img width="1270" height="720" alt="image" src="https://github.com/user-attachments/assets/76ab9080-6574-4c3c-a136-5cdde68e6d0e" /><br>
=> I2C 활성화 하면 SDA & SCL 핀 자동으로 활성화<br>



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
#define K24C256_ADDR        0xA0    // K24C256 I2C 주소 (A0,A1,A2 = 000)
#define EEPROM_PAGE_SIZE    64      // K24C256 페이지 크기 (64 bytes)
#define EEPROM_SIZE         32768   // K24C256 총 크기 (32KB)
#define TEST_ADDRESS        0x0000  // 테스트용 주소
/* USER CODE END PD */
```
```c
uint8_t i2c_scan_found = 0;
uint8_t eeprom_address = 0;
```
```c
/* USER CODE BEGIN PFP */
void I2C_Scan(void);
HAL_StatusTypeDef EEPROM_Write(uint16_t mem_addr, uint8_t *data, uint16_t size);
HAL_StatusTypeDef EEPROM_Read(uint16_t mem_addr, uint8_t *data, uint16_t size);
void EEPROM_Test(void);
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

/**
  * @brief  I2C 주소 스캔 함수
  * @param  None
  * @retval None
  */
void I2C_Scan(void)
{
  printf("\n=== I2C Address Scan ===\n");
  printf("Scanning I2C bus...\n");

  i2c_scan_found = 0;

  for(uint8_t i = 0; i < 128; i++)
  {
    if(HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i<<1), 3, 5) == HAL_OK)
    {
      printf("Found I2C device at address: 0x%02X (7-bit: 0x%02X)\n", (i<<1), i);
      i2c_scan_found++;

      // K24C256 주소 범위 확인 (0xA0~0xAE)
      if((i<<1) >= 0xA0 && (i<<1) <= 0xAE)
      {
        eeprom_address = (i<<1);
        printf("** K24C256 EEPROM detected at 0x%02X **\n", eeprom_address);
      }
    }
  }

  if(i2c_scan_found == 0)
  {
    printf("No I2C devices found!\n");
  }
  else
  {
    printf("Total %d I2C device(s) found.\n", i2c_scan_found);
  }
  printf("========================\n\n");
}

/**
  * @brief  EEPROM 쓰기 함수
  * @param  mem_addr: 메모리 주소
  * @param  data: 쓸 데이터 포인터
  * @param  size: 데이터 크기
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef EEPROM_Write(uint16_t mem_addr, uint8_t *data, uint16_t size)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint16_t bytes_to_write;
  uint16_t current_addr = mem_addr;
  uint16_t data_index = 0;

  while(size > 0)
  {
    // 페이지 경계를 고려한 쓰기 크기 계산
    bytes_to_write = EEPROM_PAGE_SIZE - (current_addr % EEPROM_PAGE_SIZE);
    if(bytes_to_write > size)
      bytes_to_write = size;

    // EEPROM에 쓰기
    status = HAL_I2C_Mem_Write(&hi2c1, eeprom_address, current_addr,
                               I2C_MEMADD_SIZE_16BIT, &data[data_index],
                               bytes_to_write, HAL_MAX_DELAY);

    if(status != HAL_OK)
    {
      printf("EEPROM Write Error at address 0x%04X\n", current_addr);
      return status;
    }

    // EEPROM 쓰기 완료 대기 (Write Cycle Time)
    HAL_Delay(5);

    // 다음 쓰기를 위한 변수 업데이트
    current_addr += bytes_to_write;
    data_index += bytes_to_write;
    size -= bytes_to_write;
  }

  return status;
}

/**
  * @brief  EEPROM 읽기 함수
  * @param  mem_addr: 메모리 주소
  * @param  data: 읽을 데이터 포인터
  * @param  size: 데이터 크기
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef EEPROM_Read(uint16_t mem_addr, uint8_t *data, uint16_t size)
{
  return HAL_I2C_Mem_Read(&hi2c1, eeprom_address, mem_addr,
                          I2C_MEMADD_SIZE_16BIT, data, size, HAL_MAX_DELAY);
}

/**
  * @brief  EEPROM 테스트 함수
  * @param  None
  * @retval None
  */
void EEPROM_Test(void)
{
  if(eeprom_address == 0)
  {
    printf("EEPROM not detected! Cannot perform test.\n\n");
    return;
  }

  printf("=== EEPROM Test ===\n");

  // 테스트 데이터 준비
  char write_data[] = "Hello, STM32F103 with K24C256 EEPROM!";
  uint8_t read_data[100] = {0};
  uint16_t data_len = strlen(write_data);

  printf("Test Address: 0x%04X\n", TEST_ADDRESS);
  printf("Write Data: \"%s\" (%d bytes)\n", write_data, data_len);

  // EEPROM에 데이터 쓰기
  printf("Writing to EEPROM...\n");
  if(EEPROM_Write(TEST_ADDRESS, (uint8_t*)write_data, data_len) == HAL_OK)
  {
    printf("Write successful!\n");
  }
  else
  {
    printf("Write failed!\n");
    return;
  }

  // 잠시 대기
  HAL_Delay(10);

  // EEPROM에서 데이터 읽기
  printf("Reading from EEPROM...\n");
  if(EEPROM_Read(TEST_ADDRESS, read_data, data_len) == HAL_OK)
  {
    printf("Read successful!\n");
    printf("Read Data: \"%s\" (%d bytes)\n", (char*)read_data, data_len);

    // 데이터 비교
    if(memcmp(write_data, read_data, data_len) == 0)
    {
      printf("** Data verification PASSED! **\n");
    }
    else
    {
      printf("** Data verification FAILED! **\n");
    }
  }
  else
  {
    printf("Read failed!\n");
  }

  printf("===================\n\n");

  // 추가 테스트: 숫자 데이터
  printf("=== Number Data Test ===\n");
  uint8_t num_write[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  uint8_t num_read[10] = {0};
  uint16_t num_addr = TEST_ADDRESS + 100;

  printf("Writing numbers 0-9 to address 0x%04X...\n", num_addr);
  if(EEPROM_Write(num_addr, num_write, 10) == HAL_OK)
  {
    HAL_Delay(10);

    if(EEPROM_Read(num_addr, num_read, 10) == HAL_OK)
    {
      printf("Write Data: ");
      for(int i = 0; i < 10; i++) printf("%d ", num_write[i]);
      printf("\n");

      printf("Read Data:  ");
      for(int i = 0; i < 10; i++) printf("%d ", num_read[i]);
      printf("\n");

      if(memcmp(num_write, num_read, 10) == 0)
      {
        printf("** Number test PASSED! **\n");
      }
      else
      {
        printf("** Number test FAILED! **\n");
      }
    }
  }
  printf("========================\n\n");
}
/* USER CODE END 0 */
```
=> I2C_Scan(): 전체 I²C 주소(0~127) 스캔 → 0xA0~0xAE 범위 감지 시 EEPROM으로 인식<br>
=> EEPROM_Write(): 16bit 주소 기반으로 데이터 쓰기
(64B 페이지 단위로 나눠 쓰고 HAL_Delay(5)로 Write Cycle 대기)<br>
=> EEPROM_Read(): 지정 주소에서 지정 크기만큼 읽기<br>
=>EEPROM_Test(): "Hello, STM32F103..." 문자열 및 숫자 배열(0~9) 쓰기·읽기·비교<br>

```c
 /* USER CODE BEGIN 2 */
  printf("\n\n");
   printf("========================================\n");
   printf("  STM32F103 I2C EEPROM K24C256 Test    \n");
   printf("  System Clock: 64MHz                  \n");
   printf("  I2C Speed: 100kHz                    \n");
   printf("========================================\n");

   // I2C 주소 스캔
   I2C_Scan();

   // EEPROM 테스트
   EEPROM_Test();

   printf("Test completed. Entering main loop...\n\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
   uint32_t loop_count = 0;

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	   // 10초마다 상태 출력
		    if(loop_count % 1000 == 0)
		    {
		      printf("System running... Loop count: %lu\n", loop_count/1000);

		      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		    }

		    loop_count++;
		    HAL_Delay(10);
  }
  /* USER CODE END 3 */
```
| 기능                 | 설명                    |
| ------------------ | --------------------- |
| **I2C_Scan()**     | 버스 연결된 모든 I²C 디바이스 검색 |
| **EEPROM_Write()** | 페이지 단위(64B)로 데이터 저장   |
| **EEPROM_Read()**  | 16bit 주소 기반 데이터 읽기    |
| **EEPROM_Test()**  | 문자열/숫자 데이터 저장 후 일치 검증 |
| **UART 출력**        | 진행 상태를 실시간 모니터링       |
| **LED 토글**         | MCU 동작 상태 확인용         | <br>






