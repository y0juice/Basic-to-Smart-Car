# 14. LDC-SPI
**기능) ST77955LCD를 SPI로 제어해 화면 초기화 및 출력** <br>
<img width="651" height="483" alt="image" src="https://github.com/user-attachments/assets/d564502c-3411-44d8-bd9e-44ba4d3aca87" />
=> PIN: GND/VCC/SCL(SCK)/SDA(MOSI)/RES/DC/CS <br>
  | **핀 이름**         | **풀네임**                           | **기능 요약**                           |
| ---------------- | --------------------------------- | ----------------------------------- |
| **SCL (SCK)**    | Serial Clock                      | 데이터 전송 타이밍을 결정하는 클록 신호              |
| **SDA (MOSI)**   | Serial Data / Master Out Slave In | MCU에서 LCD로 데이터(명령·화소정보)를 보내는 데이터 라인 |
| **RES (RST)**    | Reset                             | 디스플레이를 초기 상태로 리셋하는 신호               |
| **DC (D/C, A0)** | Data / Command                    | 전송 데이터가 명령인지, 화면 데이터인지 구분하는 신호      |
| **CS (SS)**      | Chip Select                       | SPI 통신 대상 장치를 선택하는 신호 (Low일 때 활성화)  |<br>

  +MOSI(Master Out Slave In) & MISO(Master In Slave Out): LCD는 MCU->LCD(단방향) 이므로, **MISO X**
  

__
### (1) 이론 설명 ###
<img width="908" height="372" alt="image" src="https://github.com/user-attachments/assets/e106ec1a-2121-4bfd-bcfd-a44b2dd8cf74" /><br>
<I2C vs SPI><br>
| 구분    | SPI              | I²C              |
| ----- | ---------------- | ---------------- |
| 속도    | 빠름               | 느림               |
| 선 수   | 4개 이상            | 2개(SDA, SCL)     |
| 통신 방식 | 동기식, Full Duplex | 동기식, Half Duplex |
| 주소 지정 | CS 핀으로 구분        | 주소로 구분           |
| 장점    | 빠르고 단순           | 배선 적고 슬레이브 다수 가능 |
| 단점    | 선 많음, 거리 짧음      | 속도 느림            |<br>

### (2) Pinout & Configuration

#### 01) RCC(Reset & Clcok Contorl)
<img width="1105" height="552" alt="image" src="https://github.com/user-attachments/assets/fea9eacc-5b89-47b8-8bb4-f71d4eb84576" />


#### 02)SP1 설정: SPI1 & 핀 설정
<img width="1610" height="721" alt="image" src="https://github.com/user-attachments/assets/88c51113-4170-4630-9497-e227607a3140" />
=> I2C 활성화 하면 PA5(SPI1_SICK):SPI1_SICK &PA7(SPI1_MOSI):SPI1_MOSI 핀 자동으로 활성화<br>
=> PA1(GPIO_Output):LCD_RES/ PA5(GPIO_Output):LCD_DC/ PB6(GPIO_Output):LCD_CS


___
#### Generate Code ####
___
  #### (3) 코드작성1
  -main.c 추가 코드
```c
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */
```
```c
/* USER CODE BEGIN PD */
/* USER CODE BEGIN PD */
// ST7735S Commands
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09
#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13
#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E
#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36
#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6
#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5
#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD
#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// LCD dimensions
#define LCD_WIDTH  160
#define LCD_HEIGHT 120 //80

// Colors (RGB565)
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0

/* USER CODE END PD */
```
```c
/* USER CODE BEGIN PM */
#define LCD_CS_LOW()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)
#define LCD_CS_HIGH()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)
#define LCD_DC_LOW()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET)
#define LCD_DC_HIGH()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET)
#define LCD_RES_LOW()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET)
#define LCD_RES_HIGH() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET)
/* USER CODE END PM */
```
```c
/* USER CODE BEGIN PV */
// Simple 8x8 font (ASCII 32-127) - subset for demonstration
static const uint8_t font8x8[][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ' ' (Space)
    {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00}, // '!'
    {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // '"'
    {0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00}, // '#'
    {0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00}, // '$'
    {0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00}, // '%'
    {0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00}, // '&'
    {0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00}, // '''
    {0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00}, // '('
    {0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00}, // ')'
    {0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00}, // '*'
    {0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00}, // '+'
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x06, 0x00}, // ','
    {0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00}, // '-'
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00}, // '.'
    {0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00}, // '/'
    {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00}, // '0'
    {0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00}, // '1'
    {0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00}, // '2'
    {0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00}, // '3'
    {0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00}, // '4'
    {0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00}, // '5'
    {0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00}, // '6'
    {0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00}, // '7'
    {0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00}, // '8'
    {0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00}, // '9'
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00}, // ':'
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x06, 0x00}, // ';'
    {0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00}, // '<'
    {0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00}, // '='
    {0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00}, // '>'
    {0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00}, // '?'
    {0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00}, // '@'
    {0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00}, // 'A'
    {0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00}, // 'B'
    {0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00}, // 'C'
    {0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00}, // 'D'
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00}, // 'E'
    {0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00}, // 'F'
    {0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00}, // 'G'
    {0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00}, // 'H'
    {0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // 'I'
    {0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00}, // 'J'
    {0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00}, // 'K'
    {0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00}, // 'L'
    {0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00}, // 'M'
    {0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00}, // 'N'
    {0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00}, // 'O'
    {0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00}, // 'P'
    {0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00}, // 'Q'
    {0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00}, // 'R'
    {0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00}, // 'S'
    {0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // 'T'
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00}, // 'U'
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00}, // 'V'
    {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00}, // 'W'
    {0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00}, // 'X'
    {0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00}, // 'Y'
    {0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00}, // 'Z'
    {0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00}, // '['
    {0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00}, // '\'
    {0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00}, // ']'
    {0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00}, // '^'
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF}, // '_'
    {0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00}, // '`'
    {0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00}, // 'a'
    {0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00}, // 'b'
    {0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00}, // 'c'
    {0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00}, // 'd'
    {0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00}, // 'e'
    {0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00}, // 'f'
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F}, // 'g'
    {0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00}, // 'h'
    {0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // 'i'
    {0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E}, // 'j'
    {0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00}, // 'k'
    {0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, // 'l'
    {0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00}, // 'm'
    {0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00}, // 'n'
    {0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00}, // 'o'
    {0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F}, // 'p'
    {0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78}, // 'q'
    {0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00}, // 'r'
    {0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00}, // 's'
    {0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00}, // 't'
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00}, // 'u'
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00}, // 'v'
    {0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00}, // 'w'
    {0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00}, // 'x'
    {0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F}, // 'y'
    {0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00}, // 'z'
    {0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00}, // '{'
    {0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00}, // '|'
    {0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00}, // '}'
    {0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // '~'
};

/* USER CODE END PV */
```

```c
/* USER CODE BEGIN 0 */

void LCD_WriteCommand(uint8_t cmd) {
    LCD_CS_LOW();
    LCD_DC_LOW();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    LCD_CS_HIGH();
}

void LCD_WriteData(uint8_t data) {
    LCD_CS_LOW();
    LCD_DC_HIGH();
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    LCD_CS_HIGH();
}

void LCD_WriteData16(uint16_t data) {
    uint8_t buffer[2];
    buffer[0] = (data >> 8) & 0xFF;
    buffer[1] = data & 0xFF;

    LCD_CS_LOW();
    LCD_DC_HIGH();
    HAL_SPI_Transmit(&hspi1, buffer, 2, HAL_MAX_DELAY);
    LCD_CS_HIGH();
}

void LCD_Init(void) {
    // Hardware reset
    LCD_RES_LOW();
    HAL_Delay(100);
    LCD_RES_HIGH();
    HAL_Delay(100);

    // Software reset
    LCD_WriteCommand(ST7735_SWRESET);
    HAL_Delay(150);

    // Out of sleep mode
    LCD_WriteCommand(ST7735_SLPOUT);
    HAL_Delay(500);

    // Frame rate control - normal mode
    LCD_WriteCommand(ST7735_FRMCTR1);
    LCD_WriteData(0x01);
    LCD_WriteData(0x2C);
    LCD_WriteData(0x2D);

    // Frame rate control - idle mode
    LCD_WriteCommand(ST7735_FRMCTR2);
    LCD_WriteData(0x01);
    LCD_WriteData(0x2C);
    LCD_WriteData(0x2D);

    // Frame rate control - partial mode
    LCD_WriteCommand(ST7735_FRMCTR3);
    LCD_WriteData(0x01);
    LCD_WriteData(0x2C);
    LCD_WriteData(0x2D);
    LCD_WriteData(0x01);
    LCD_WriteData(0x2C);
    LCD_WriteData(0x2D);

    // Display inversion control
    LCD_WriteCommand(ST7735_INVCTR);
    LCD_WriteData(0x07);

    // Power control
    LCD_WriteCommand(ST7735_PWCTR1);
    LCD_WriteData(0xA2);
    LCD_WriteData(0x02);
    LCD_WriteData(0x84);

    LCD_WriteCommand(ST7735_PWCTR2);
    LCD_WriteData(0xC5);

    LCD_WriteCommand(ST7735_PWCTR3);
    LCD_WriteData(0x0A);
    LCD_WriteData(0x00);

    LCD_WriteCommand(ST7735_PWCTR4);
    LCD_WriteData(0x8A);
    LCD_WriteData(0x2A);

    LCD_WriteCommand(ST7735_PWCTR5);
    LCD_WriteData(0x8A);
    LCD_WriteData(0xEE);

    // VCOM control
    LCD_WriteCommand(ST7735_VMCTR1);
    LCD_WriteData(0x0E);

    // Display inversion off
    LCD_WriteCommand(ST7735_INVOFF);

    // Memory access control (rotation)
    LCD_WriteCommand(ST7735_MADCTL);
    // 1. 기본 90도 회전 (추천)
    //LCD_WriteData(0x20); // MY=0, MX=0, MV=1
    // 2. 현재 사용중
    //LCD_WriteData(0xE0); // MY=1, MX=1, MV=1
    // 3. 90도 + X축만 미러링
    LCD_WriteData(0x60); // MY=0, MX=1, MV=1
    // 4. 90도 + Y축만 미러링
    //LCD_WriteData(0xA0); // MY=1, MX=0, MV=1

    // Color mode: 16-bit color
    LCD_WriteCommand(ST7735_COLMOD);
    LCD_WriteData(0x05);

    // Column address set
    LCD_WriteCommand(ST7735_CASET);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    //LCD_WriteData(0x4F); // 79
    LCD_WriteData(0x9F); // 159


    // Row address set
    LCD_WriteCommand(ST7735_RASET);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    //LCD_WriteData(0x9F); // 159
    // Row address set (80픽셀)
	LCD_WriteData(0x4F); // 79

    // Gamma correction
    LCD_WriteCommand(ST7735_GMCTRP1);
    LCD_WriteData(0x0f);
    LCD_WriteData(0x1a);
    LCD_WriteData(0x0f);
    LCD_WriteData(0x18);
    LCD_WriteData(0x2f);
    LCD_WriteData(0x28);
    LCD_WriteData(0x20);
    LCD_WriteData(0x22);
    LCD_WriteData(0x1f);
    LCD_WriteData(0x1b);
    LCD_WriteData(0x23);
    LCD_WriteData(0x37);
    LCD_WriteData(0x00);
    LCD_WriteData(0x07);
    LCD_WriteData(0x02);
    LCD_WriteData(0x10);

    LCD_WriteCommand(ST7735_GMCTRN1);
    LCD_WriteData(0x0f);
    LCD_WriteData(0x1b);
    LCD_WriteData(0x0f);
    LCD_WriteData(0x17);
    LCD_WriteData(0x33);
    LCD_WriteData(0x2c);
    LCD_WriteData(0x29);
    LCD_WriteData(0x2e);
    LCD_WriteData(0x30);
    LCD_WriteData(0x30);
    LCD_WriteData(0x39);
    LCD_WriteData(0x3f);
    LCD_WriteData(0x00);
    LCD_WriteData(0x07);
    LCD_WriteData(0x03);
    LCD_WriteData(0x10);

    // Normal display on
    LCD_WriteCommand(ST7735_NORON);
    HAL_Delay(10);

    // Main screen turn on
    LCD_WriteCommand(ST7735_DISPON);
    HAL_Delay(100);
}

void LCD_SetWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    // 0.96" ST7735S LCD 오프셋 적용
    uint8_t x_offset = 0;  // X축 오프셋
    uint8_t y_offset = 0;   // Y축 오프셋

    // Column address set (X축)
    LCD_WriteCommand(ST7735_CASET);
    LCD_WriteData(0x00);
    LCD_WriteData(x0 + x_offset);
    LCD_WriteData(0x00);
    LCD_WriteData(x1 + x_offset);

    // Row address set (Y축)
    LCD_WriteCommand(ST7735_RASET);
    LCD_WriteData(0x00);
    LCD_WriteData(y0 + y_offset);
    LCD_WriteData(0x00);
    LCD_WriteData(y1 + y_offset);

    // Write to RAM
    LCD_WriteCommand(ST7735_RAMWR);
}

void LCD_DrawPixel(uint8_t x, uint8_t y, uint16_t color) {
    if(x >= LCD_WIDTH || y >= LCD_HEIGHT) return;

    LCD_SetWindow(x, y, x, y);
    LCD_WriteData16(color);
}

void LCD_Fill(uint16_t color) {
    LCD_SetWindow(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);

    LCD_CS_LOW();
    LCD_DC_HIGH();

    for(uint16_t i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
        uint8_t buffer[2];
        buffer[0] = (color >> 8) & 0xFF;
        buffer[1] = color & 0xFF;
        HAL_SPI_Transmit(&hspi1, buffer, 2, HAL_MAX_DELAY);
    }

    LCD_CS_HIGH();
}

void LCD_DrawChar(uint8_t x, uint8_t y, char ch, uint16_t color, uint16_t bg_color) {
    if(ch < 32 || ch > 126) ch = 32; // Replace invalid chars with space

    const uint8_t* font_char = font8x8[ch - 32];

    for(uint8_t i = 0; i < 8; i++) {
        uint8_t line = font_char[i];
        for(uint8_t j = 0; j < 8; j++) {
            //if(line & (0x80 >> j)) {
        	if(line & (0x01 << j)) { // LSB부터 읽기
                LCD_DrawPixel(x + j, y + i, color);
            } else {
                LCD_DrawPixel(x + j, y + i, bg_color);
            }
        }
    }
}

void LCD_DrawString(uint8_t x, uint8_t y, const char* str, uint16_t color, uint16_t bg_color) {
    uint8_t orig_x = x;

    while(*str) {
        if(*str == '\n') {
            y += 8;
            x = orig_x;
        } else if(*str == '\r') {
            x = orig_x;
        } else {
            if(x + 8 > LCD_WIDTH) {
                x = orig_x;
                y += 8;
            }
            if(y + 8 > LCD_HEIGHT) {
                break;
            }

            LCD_DrawChar(x, y, *str, color, bg_color);
            x += 8;
        }
        str++;
    }
}

/* USER CODE END 0 */
```
=>LOW_DC_HIGH(): 데이터 전송모드/LOW_DC_LOW(): 데이터 명령모드<br>
```c
 /* USER CODE BEGIN 2 */
  // Initialize LCD
  LCD_Init();//필수

  // Clear screen with black background
  LCD_Fill(BLACK);// lcd 잔상지우기

  LCD_DrawString(10, 30, "Um Min Ho Ddong", WHITE, BLACK);
  LCD_DrawString(10, 45, "Jae Min Kim, Babo ", GREEN, BLACK);
  LCD_DrawString(10, 60, "+67", CYAN, BLACK);
  LCD_DrawString(10, 75, "Our Friendship Foever", YELLOW, BLACK);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  HAL_Delay(1000);

  }
  /* USER CODE END 3 */
```
----
**위랑 동일 알고리즘이나, main.c에서 lcd.h & lcd.c 분리<ㅠㄱ>**
  #### (3) 코드작성2
  -lcd.h
```c
#ifndef __LCD_H
#define __LCD_H

#include "main.h"
#include <stdint.h>




// LCD dimensions
#define LCD_WIDTH  160
#define LCD_HEIGHT 120

// Colors
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0


// LCD functions
void LCD_Init(void);
void LCD_Fill(uint16_t color);
void LCD_DrawPixel(uint8_t x, uint8_t y, uint16_t color);
void LCD_DrawChar(uint8_t x, uint8_t y, char ch, uint16_t color, uint16_t bg_color);
void LCD_DrawString(uint8_t x, uint8_t y, const char* str, uint16_t color, uint16_t bg_color);


#endif
```
-lcd.c
```c
#include "lcd.h"
#include "main.h" // For HAL_SPI_Transmit, HAL_Delay, HAL_GPIO_WritePin etc.
#include <string.h>

// SPI 핸들은 main.c에 선언되어있는 것을 사용합니다.
// master.c 프로젝트에 SPI_HandleTypeDef hspi1; 변수가 있는지 확인해주세요.
// 만약 hspi2 등 다른 변수를 사용한다면 이 부분을 수정해야 합니다.
extern SPI_HandleTypeDef hspi1;

/* Private define ------------------------------------------------------------*/
// ST7735S Commands
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09
#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13
#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E
#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36
#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6
#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5
#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD
#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

/* Private macro -------------------------------------------------------------*/
// GPIO macros
#define LCD_CS_LOW()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)
#define LCD_CS_HIGH()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)
#define LCD_DC_LOW()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET)
#define LCD_DC_HIGH()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET)
#define LCD_RES_LOW()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET)
#define LCD_RES_HIGH() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET)

/* Private variables ---------------------------------------------------------*/
static const uint8_t font8x8[][8] = { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00 }, // ' ' (Space)
      { 0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00 }, // '!'
      { 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // '"'
      { 0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00 }, // '#'
      { 0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00 }, // '$'
      { 0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00 }, // '%'
      { 0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00 }, // '&'
      { 0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00 }, // '''
      { 0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00 }, // '('
      { 0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00 }, // ')'
      { 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00 }, // '*'
      { 0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00 }, // '+'
      { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x06, 0x00 }, // ','
      { 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00 }, // '-'
      { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00 }, // '.'
      { 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00 }, // '/'
      { 0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00 }, // '0'
      { 0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00 }, // '1'
      { 0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00 }, // '2'
      { 0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00 }, // '3'
      { 0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00 }, // '4'
      { 0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00 }, // '5'
      { 0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00 }, // '6'
      { 0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00 }, // '7'
      { 0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00 }, // '8'
      { 0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00 }, // '9'
      { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00 }, // ':'
      { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x06, 0x00 }, // ';'
      { 0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00 }, // '<'
      { 0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00 }, // '='
      { 0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00 }, // '>'
      { 0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00 }, // '?'
      { 0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00 }, // '@'
      { 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00 }, // 'A'
      { 0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00 }, // 'B'
      { 0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00 }, // 'C'
      { 0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00 }, // 'D'
      { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00 }, // 'E'
      { 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00 }, // 'F'
      { 0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00 }, // 'G'
      { 0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00 }, // 'H'
      { 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00 }, // 'I'
      { 0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00 }, // 'J'
      { 0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00 }, // 'K'
      { 0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00 }, // 'L'
      { 0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00 }, // 'M'
      { 0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00 }, // 'N'
      { 0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00 }, // 'O'
      { 0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00 }, // 'P'
      { 0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00 }, // 'Q'
      { 0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00 }, // 'R'
      { 0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00 }, // 'S'
      { 0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00 }, // 'T'
      { 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00 }, // 'U'
      { 0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00 }, // 'V'
      { 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00 }, // 'W'
      { 0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00 }, // 'X'
      { 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00 }, // 'Y'
      { 0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00 }, // 'Z'
      { 0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00 }, // '['
      { 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00 }, // '\'
      { 0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00 }, // ']'
      { 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00 }, // '^'
      { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF }, // '_'
      { 0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00 }, // '`'
      { 0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00 }, // 'a'
      { 0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00 }, // 'b'
      { 0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00 }, // 'c'
      { 0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00 }, // 'd'
      { 0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00 }, // 'e'
      { 0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00 }, // 'f'
      { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F }, // 'g'
      { 0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00 }, // 'h'
      { 0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00 }, // 'i'
      { 0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E }, // 'j'
      { 0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00 }, // 'k'
      { 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00 }, // 'l'
      { 0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00 }, // 'm'
      { 0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00 }, // 'n'
      { 0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00 }, // 'o'
      { 0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F }, // 'p'
      { 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78 }, // 'q'
      { 0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00 }, // 'r'
      { 0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00 }, // 's'
      { 0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00 }, // 't'
      { 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00 }, // 'u'
      { 0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00 }, // 'v'
      { 0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00 }, // 'w'
      { 0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00 }, // 'x'
      { 0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F }, // 'y'
      { 0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00 }, // 'z'
      { 0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00 }, // '{'
      { 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00 }, // '|'
      { 0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00 }, // '}'
//      { 0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // '~'
      };

/* Private function prototypes -----------------------------------------------*/
static void LCD_WriteCommand(uint8_t cmd);
static void LCD_WriteData(uint8_t data);
static void LCD_WriteData16(uint16_t data);
static void LCD_SetWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

/* Function implementations --------------------------------------------------*/

static void LCD_WriteCommand(uint8_t cmd) {
    LCD_CS_LOW();
    LCD_DC_LOW();
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    LCD_CS_HIGH();
}

static void LCD_WriteData(uint8_t data) {
    LCD_CS_LOW();
    LCD_DC_HIGH();
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    LCD_CS_HIGH();
}

static void LCD_WriteData16(uint16_t data) {
    uint8_t buffer[2];
    buffer[0] = (data >> 8) & 0xFF;
    buffer[1] = data & 0xFF;

    LCD_CS_LOW();
    LCD_DC_HIGH();
    HAL_SPI_Transmit(&hspi1, buffer, 2, HAL_MAX_DELAY);
    LCD_CS_HIGH();
}

void LCD_Init(void) {
   // Hardware reset
   LCD_RES_LOW();
   HAL_Delay(100);
   LCD_RES_HIGH();
   HAL_Delay(100);

   // Software reset
   LCD_WriteCommand(ST7735_SWRESET);
   HAL_Delay(150);

   // Out of sleep mode
   LCD_WriteCommand(ST7735_SLPOUT);
   HAL_Delay(500);

   // Frame rate control - normal mode
   LCD_WriteCommand(ST7735_FRMCTR1);
   LCD_WriteData(0x01);
   LCD_WriteData(0x2C);
   LCD_WriteData(0x2D);

   // Frame rate control - idle mode
   LCD_WriteCommand(ST7735_FRMCTR2);
   LCD_WriteData(0x01);
   LCD_WriteData(0x2C);
   LCD_WriteData(0x2D);

   // Frame rate control - partial mode
   LCD_WriteCommand(ST7735_FRMCTR3);
   LCD_WriteData(0x01);
   LCD_WriteData(0x2C);
   LCD_WriteData(0x2D);
   LCD_WriteData(0x01);
   LCD_WriteData(0x2C);
   LCD_WriteData(0x2D);

   // Display inversion control
   LCD_WriteCommand(ST7735_INVCTR);
   LCD_WriteData(0x07);

   // Power control
   LCD_WriteCommand(ST7735_PWCTR1);
   LCD_WriteData(0xA2);
   LCD_WriteData(0x02);
   LCD_WriteData(0x84);

   LCD_WriteCommand(ST7735_PWCTR2);
   LCD_WriteData(0xC5);

   LCD_WriteCommand(ST7735_PWCTR3);
   LCD_WriteData(0x0A);
   LCD_WriteData(0x00);

   LCD_WriteCommand(ST7735_PWCTR4);
   LCD_WriteData(0x8A);
   LCD_WriteData(0x2A);

   LCD_WriteCommand(ST7735_PWCTR5);
   LCD_WriteData(0x8A);
   LCD_WriteData(0xEE);

   // VCOM control
   LCD_WriteCommand(ST7735_VMCTR1);
   LCD_WriteData(0x0E);

   // Display inversion off
   LCD_WriteCommand(ST7735_INVOFF);

   // Memory access control (rotation)
   LCD_WriteCommand(ST7735_MADCTL);
   // 1. 기본 90도 회전 (추천)
   //LCD_WriteData(0x20); // MY=0, MX=0, MV=1
   // 2. 현재 사용중
   //LCD_WriteData(0xE0); // MY=1, MX=1, MV=1
   // 3. 90도 + X축만 미러링
   LCD_WriteData(0x60); // MY=0, MX=1, MV=1
   // 4. 90도 + Y축만 미러링
   //LCD_WriteData(0xA0); // MY=1, MX=0, MV=1

   // Color mode: 16-bit color
   LCD_WriteCommand(ST7735_COLMOD);
   LCD_WriteData(0x05);

   // Column address set
   LCD_WriteCommand(ST7735_CASET);
   LCD_WriteData(0x00);
   LCD_WriteData(0x00);
   LCD_WriteData(0x00);
   //LCD_WriteData(0x4F); // 79
   LCD_WriteData(0x9F); // 159

   // Row address set
   LCD_WriteCommand(ST7735_RASET);
   LCD_WriteData(0x00);
   LCD_WriteData(0x00);
   LCD_WriteData(0x00);
   //LCD_WriteData(0x9F); // 159
   // Row address set (80픽셀)
   LCD_WriteData(0x4F); // 79

   // Gamma correction
   LCD_WriteCommand(ST7735_GMCTRP1);
   LCD_WriteData(0x0f);
   LCD_WriteData(0x1a);
   LCD_WriteData(0x0f);
   LCD_WriteData(0x18);
   LCD_WriteData(0x2f);
   LCD_WriteData(0x28);
   LCD_WriteData(0x20);
   LCD_WriteData(0x22);
   LCD_WriteData(0x1f);
   LCD_WriteData(0x1b);
   LCD_WriteData(0x23);
   LCD_WriteData(0x37);
   LCD_WriteData(0x00);
   LCD_WriteData(0x07);
   LCD_WriteData(0x02);
   LCD_WriteData(0x10);

   LCD_WriteCommand(ST7735_GMCTRN1);
   LCD_WriteData(0x0f);
   LCD_WriteData(0x1b);
   LCD_WriteData(0x0f);
   LCD_WriteData(0x17);
   LCD_WriteData(0x33);
   LCD_WriteData(0x2c);
   LCD_WriteData(0x29);
   LCD_WriteData(0x2e);
   LCD_WriteData(0x30);
   LCD_WriteData(0x30);
   LCD_WriteData(0x39);
   LCD_WriteData(0x3f);
   LCD_WriteData(0x00);
   LCD_WriteData(0x07);
   LCD_WriteData(0x03);
   LCD_WriteData(0x10);

   // Normal display on
   LCD_WriteCommand(ST7735_NORON);
   HAL_Delay(10);

   // Main screen turn on
   LCD_WriteCommand(ST7735_DISPON);
   HAL_Delay(100);
}

void LCD_SetWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
   // 0.96" ST7735S LCD 오프셋 적용
   uint8_t x_offset = 0;  // X축 오프셋
   uint8_t y_offset = 0;   // Y축 오프셋

   // Column address set (X축)
   LCD_WriteCommand(ST7735_CASET);
   LCD_WriteData(0x00);
   LCD_WriteData(x0 + x_offset);
   LCD_WriteData(0x00);
   LCD_WriteData(x1 + x_offset);

   // Row address set (Y축)
   LCD_WriteCommand(ST7735_RASET);
   LCD_WriteData(0x00);
   LCD_WriteData(y0 + y_offset);
   LCD_WriteData(0x00);
   LCD_WriteData(y1 + y_offset);

   // Write to RAM
   LCD_WriteCommand(ST7735_RAMWR);
}

void LCD_DrawPixel(uint8_t x, uint8_t y, uint16_t color) {
   if (x >= LCD_WIDTH || y >= LCD_HEIGHT)
      return;

   LCD_SetWindow(x, y, x, y);
   LCD_WriteData16(color);
}

void LCD_Fill(uint16_t color) {
   LCD_SetWindow(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

   LCD_CS_LOW();
   LCD_DC_HIGH();

   for (uint16_t i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
      uint8_t buffer[2];
      buffer[0] = (color >> 8) & 0xFF;
      buffer[1] = color & 0xFF;
      HAL_SPI_Transmit(&hspi1, buffer, 2, HAL_MAX_DELAY);
   }

   LCD_CS_HIGH();
}

void LCD_DrawChar(uint8_t x, uint8_t y, char ch, uint16_t color,
      uint16_t bg_color) {
   if (ch < 32 || ch > 126)
      ch = 32; // Replace invalid chars with space

   const uint8_t *font_char = font8x8[ch - 32];

   for (uint8_t i = 0; i < 8; i++) {
      uint8_t line = font_char[i];
      for (uint8_t j = 0; j < 8; j++) {
         //if(line & (0x80 >> j)) {
         if (line & (0x01 << j)) { // LSB부터 읽기
            LCD_DrawPixel(x + j, y + i, color);
         } else {
            LCD_DrawPixel(x + j, y + i, bg_color);
         }
      }
   }
}

void LCD_DrawString(uint8_t x, uint8_t y, const char *str, uint16_t color,
      uint16_t bg_color) {
   uint8_t orig_x = x;

   while (*str) {
      if (*str == '\n') {
         y += 8;
         x = orig_x;
      } else if (*str == '\r') {
         x = orig_x;
      } else {
         if (x + 8 > LCD_WIDTH) {
            x = orig_x;
            y += 8;
         }
         if (y + 8 > LCD_HEIGHT) {
            break;
         }

         LCD_DrawChar(x, y, *str, color, bg_color);
         x += 8;
      }
      str++;
   }
}

```
  -main.c 추가 코드
  ```c
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "lcd.h"
//#include "stm32f1xx_hal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
// Simple 8x8 font (ASCII 32-127) - subset for demonstration

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
// LCD function prototypes


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE BEGIN 0 */



/* USER CODE END 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  // Initialize LCD
  LCD_Init();
//  LCD_DrawString(10, 30, "Um Min Ho Ddong", WHITE, BLACK);


  // Clear screen with black background
  LCD_Fill(BLACK);

  LCD_DrawString(10, 30, "Um Min Ho Ddong", WHITE, BLACK);
  LCD_DrawString(10, 45, "Jae Min Kim, Babo ", GREEN, BLACK);
  LCD_DrawString(10, 60, "+67", CYAN, BLACK);
  LCD_DrawString(10, 75, "Our Friendship Foever", YELLOW, BLACK);
//  LCD_DeleteArea(10, 75, LCD_WIDTH-1,90, BLACK);
  LCD_DrawString(10, 60, "Jae Min Kim, Babo ", GREEN, BLACK);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  HAL_Delay(1000);

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_RES_Pin|LCD_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RES_Pin LCD_DC_Pin */
  GPIO_InitStruct.Pin = LCD_RES_Pin|LCD_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_CS_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
```

  
