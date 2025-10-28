# 8. CLCD
**기능) terminal 창이 아닌 실제 LCD을 통해 문자 출력 + HW 연결 check: I2C 방식 사용**<br>
<br>

__
### (1) 이론 설명 ###
<img width="861" height="188" alt="image" src="https://github.com/user-attachments/assets/33da9a4f-0211-42c0-be76-59e1f109688a" /><br>
- SCL(Serial Clock Line): 데이터를 주고 받을 때 타이밍을 주는 clk 신호<br>
- SDA(Serial Data Line): 실제 데이터 신호<br>
- Connectivity: **I2C1** 활성화
   | 구분                 | I²C (아이투씨)                | UART (유아트, `huart`)                         | SPI (에스피아이)                 |
| ------------------ | ------------------------- | ------------------------------------------- | --------------------------- |
| **의미**             | Inter-Integrated Circuit  | Universal Asynchronous Receiver/Transmitter | Serial Peripheral Interface |
| **통신선 개수**         | 2개 (SCL, SDA)             | 2개 (TX, RX)                                 | 최소 4개 (MOSI, MISO, SCK, SS) |
| **통신 방식**          | 동기식 (Clock 사용)            | 비동기식 (Clock 없음)                             | 동기식 (Clock 사용)              |
| **전송 속도**          | 보통 100k~400kHz            | 수백 kbps~Mbps                                | 수 MHz~수십 MHz (가장 빠름)        |
| **연결 구조**          | 1개 마스터 + 여러 슬레이브          | 1:1 통신                                      | 1개 마스터 + 여러 슬레이브 (SS로 선택)   |
| **대표 사용 예시**       | LCD, EEPROM, 센서(BMP280 등) | 블루투스, 시리얼 통신, PC 터미널                        | OLED, SD카드, 고속 센서, TFT LCD  |
| **STM32에서 핸들러 이름** | `hi2c1`                   | `huart1`                                    | `hspi1`                     |

### (2) Pinout & Configuration

#### 01) RCC(Reset & Clcok Contorl)
<img width="1105" height="552" alt="image" src="https://github.com/user-attachments/assets/fea9eacc-5b89-47b8-8bb4-f71d4eb84576" /><br>


#### 02) Connectiviy: I2C 설정
<img width="713" height="458" alt="image" src="https://github.com/user-attachments/assets/299af76f-fdce-4926-a7ed-7b682a61b426" />



  #### 03) 핀 패정
<img width="1534" height="733" alt="image" src="https://github.com/user-attachments/assets/bc6593f7-4eee-495c-b8bd-100735cc2b90" /><br>


- I2C1_SDA(**PB9**):D9(IO_13)<br>
- I2C1_SCL(**PB8**):A3(IO-34)<br>
=> I2C, uhart, SPI이 모두 사용할 수 있는 핀 정해져있음<br>
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
=> printf 허용
```c
/* USER CODE BEGIN PD */
#define delay_ms HAL_Delay

#define ADDRESS   0x3F << 1
//#define ADDRESS   0x27 << 1

#define RS1_EN1   0x05 //0000 0101 RS=1, EN=1
#define RS1_EN0   0x01// 0000 0001 Rs=1, EN=0
#define RS0_EN1   0x04
#define RS0_EN0   0x00
#define BackLight 0x08
/* USER CODE END PD */
```
=> RS: 데이터(1) or 명령(0) 구분<br>

=> EN: Enable 펄스 (1→0으로 내려가며 실행)<br>

=> BackLight: 백라이트 ON (1)<br>


```c
/* USER CODE PV */
int delay = 0;
int value = 0;

/* USER CODE END PV */

```


```c
/* USER CODE BEGIN PFP */
void I2C_ScanAddresses(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
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

void I2C_ScanAddresses(void) {
    HAL_StatusTypeDef result;
    uint8_t i;

    printf("Scanning I2C addresses...\r\n");

    for (i = 1; i < 128; i++) {
        /*
         * HAL_I2C_IsDeviceReady: If a device at the specified address exists return HAL_OK.
         * Since I2C devices must have an 8-bit address, the 7-bit address is shifted left by 1 bit.
         */
        result = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i << 1), 1, 10);
        if (result == HAL_OK) {
            printf("I2C device found at address 0x%02X\r\n", i);
        }
    }

    printf("Scan complete.\r\n");
}

void delay_us(int us){
	value = 3;
	delay = us * value;
	for(int i=0;i < delay;i++);
}

void LCD_DATA(uint8_t data) {
	uint8_t temp=(data & 0xF0)|RS1_EN1|BackLight;

	while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
	temp=(data & 0xF0)|RS1_EN0|BackLight;
	while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
	delay_us(4);

	temp=((data << 4) & 0xF0)|RS1_EN1|BackLight;
	while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
	temp = ((data << 4) & 0xF0)|RS1_EN0|BackLight;
	while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
	delay_us(50);
}

void LCD_CMD(uint8_t cmd) {
	uint8_t temp=(cmd & 0xF0)|RS0_EN1|BackLight;
	while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
	temp=(cmd & 0xF0)|RS0_EN0|BackLight;
	while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
	delay_us(4);

	temp=((cmd << 4) & 0xF0)|RS0_EN1|BackLight;
	while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
	temp=((cmd << 4) & 0xF0)|RS0_EN0|BackLight;
	while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
	delay_us(50);
}

void LCD_CMD_4bit(uint8_t cmd) {
	uint8_t temp=((cmd << 4) & 0xF0)|RS0_EN1|BackLight;
	while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
	temp=((cmd << 4) & 0xF0)|RS0_EN0|BackLight;
	while(HAL_I2C_Master_Transmit(&hi2c1, ADDRESS, &temp, 1, 1000)!=HAL_OK);
	delay_us(50);
}

void LCD_INIT(void) {

	delay_ms(100);

	LCD_CMD_4bit(0x03); delay_ms(5);
	LCD_CMD_4bit(0x03); delay_us(100);
	LCD_CMD_4bit(0x03); delay_us(100);
	LCD_CMD_4bit(0x02); delay_us(100);
	LCD_CMD(0x28);  // 4 bits, 2 line, 5x8 font
	LCD_CMD(0x08);  // display off, cursor off, blink off
	LCD_CMD(0x01);  // clear display
	delay_ms(3);
	LCD_CMD(0x06);  // cursor movint direction
	LCD_CMD(0x0C);  // display on, cursor off, blink off
}

void LCD_XY(char x, char y) {
	if      (y == 0) LCD_CMD(0x80 + x);
	else if (y == 1) LCD_CMD(0xC0 + x);
	else if (y == 2) LCD_CMD(0x94 + x);
	else if (y == 3) LCD_CMD(0xD4 + x);
}

void LCD_CLEAR(void) {
	LCD_CMD(0x01);
	delay_ms(2);
}

void LCD_PUTS(char *str) {
	while (*str) LCD_DATA(*str++);
}

/* USER CODE END 0 */
```
=> I2C_ScanAddresses(): 장치 검색(하드웨어 연결 확인용)<br>
=> LCD_DATA(): 문자 데이터 전송<br>
=> LCD_CMD(): 명령 전송<br>
=> LCD_CMD_4bit(): 8bit에서 4bit로 전환<br>

```c
/* USER CODE BEGIN 2 */
  I2C_ScanAddresses();

    LCD_INIT();
    LCD_XY(0, 0) ; LCD_PUTS((char *)"LCD Display test");
    LCD_XY(0, 1) ; LCD_PUTS((char *)"Hello World.....");


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
```
