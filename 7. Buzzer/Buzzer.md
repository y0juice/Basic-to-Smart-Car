# 7. Buzzer
**기능) 초음파센서로 거리측정**<br>
<img width="780" height="614" alt="image" src="https://github.com/user-attachments/assets/e4091a1d-72b3-49b4-b9c8-4dd4335f12ec" />
<br>

__
### (1) 이론 설명 ###
<img width="416" height="314" alt="image" src="https://github.com/user-attachments/assets/3f45a89f-2136-480d-a322-03e2320fd01a" /><br>
=> Vcc: 3.5[V] : 소리가 더큼br>
<img width="967" height="776" alt="image" src="https://github.com/user-attachments/assets/f11b7e43-79b8-443d-b7ca-3825838f62de" />


### (2) Pinout & Configuration

#### 01) RCC(Reset & Clcok Contorl)
<img width="1105" height="552" alt="image" src="https://github.com/user-attachments/assets/fea9eacc-5b89-47b8-8bb4-f71d4eb84576" /><br>


#### 02) TIM1 설정
<img width="786" height="763" alt="image" src="https://github.com/user-attachments/assets/65480865-0b38-45df-a79e-4bab4cd55653" /><br>

- 정밀도가 더 높은 TIM1 사용
- Channel1 -> PWM Geneeration CH1<br>
- Configuration → Parameter Settings:
   *Prescaler: 64MHz ÷ 64 (1MHz 클록)<br>
     => Prescarler를 64로 설정: 타이머 공급(64,000,000Hz)/64= 1,000,000 Hz 클럭 공급<br>
                           : 1초에 1,000,000 클럭인가 = 1clk 당, 1/1,000,000초 소요<br>
   
   *Counter Period: 1000 (초기값, 코드에서 동적 변경)<br>
    => Counter Period를 1000으로 설정: 인터럽트 주기 1/1,000,000초 * 65536<br>
   *Pulse: 500 (50% duty cycle)
  
  #### 03) 핀 패정

<img width="928" height="1536" alt="image" src="https://github.com/user-attachments/assets/a3846325-cbe7-4fbb-b305-26979854f022" /><br>

- TIM1_CH1(**PA9**)<br>
___
#### Generate Code ####
___
  #### (3) 코드작성
  -main.c 추가 코드
```c
/* USER CODE BEGIN Includes */
int start_music_played = 0;
/* USER CODE END Includes */
```

```c
/* USER CODE BEGIN PD */
// 음표 주파수 정의 (Hz)
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760//
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520//
#define NOTE_AS7 3729
#define NOTE_B7  3951

#define REST 0


// 음표 길이 정의 (밀리초)
#define WHOLE     1400      // 원래 2000
#define HALF      700       // 원래 1000
#define QUARTER   350       // 원래 500
#define EIGHTH    175       // 원래 250
#define SIXTEENTH 90        // 원래 125

//#define EIGHTH    175       // 원래 250
//#define SIXTEENTH 250       // 원래 125

/* USER CODE END PD */
```

```c
/* USER CODE BEGIN PV */
typedef struct {
    uint16_t frequency;
    uint16_t duration;
} Note; // 한음을 어떻게 표현할지지

const Note mario_theme[] = {
		 {NOTE_E7, EIGHTH}, {NOTE_E7, EIGHTH}, {REST, EIGHTH}, {NOTE_E7, EIGHTH},
		    {REST, EIGHTH}, {NOTE_C7, EIGHTH}, {NOTE_E7, EIGHTH}, {REST, EIGHTH},
		    {NOTE_G7, QUARTER}, {REST, QUARTER}, {NOTE_G6, QUARTER}, {REST, QUARTER},

		    // 두 번째 구간
		    {NOTE_C7, QUARTER}, {REST, EIGHTH}, {NOTE_G6, EIGHTH}, {REST, EIGHTH},
		    {NOTE_E6, QUARTER}, {REST, EIGHTH}, {NOTE_A6, EIGHTH}, {REST, EIGHTH},
		    {NOTE_B6, EIGHTH}, {REST, EIGHTH}, {NOTE_AS6, EIGHTH}, {NOTE_A6, QUARTER},

		    // 세 번째 구간
		    {NOTE_G6, EIGHTH}, {NOTE_E7, EIGHTH}, {NOTE_G7, EIGHTH}, {NOTE_A7, QUARTER},
		    {NOTE_F7, EIGHTH}, {NOTE_G7, EIGHTH}, {REST, EIGHTH}, {NOTE_E7, EIGHTH},
		    {REST, EIGHTH}, {NOTE_C7, EIGHTH}, {NOTE_D7, EIGHTH}, {NOTE_B6, QUARTER},

		    // 반복 구간
		    {NOTE_C7, QUARTER}, {REST, EIGHTH}, {NOTE_G6, EIGHTH}, {REST, EIGHTH},
		    {NOTE_E6, QUARTER}, {REST, EIGHTH}, {NOTE_A6, EIGHTH}, {REST, EIGHTH},
		    {NOTE_B6, EIGHTH}, {REST, EIGHTH}, {NOTE_AS6, EIGHTH}, {NOTE_A6, QUARTER},

		    {NOTE_G6, EIGHTH}, {NOTE_E7, EIGHTH}, {NOTE_G7, EIGHTH}, {NOTE_A7, QUARTER},
		    {NOTE_F7, EIGHTH}, {NOTE_G7, EIGHTH}, {REST, EIGHTH}, {NOTE_E7, EIGHTH},
		    {REST, EIGHTH}, {NOTE_C7, EIGHTH}, {NOTE_D7, EIGHTH}, {NOTE_B6, QUARTER},

		    // 마무리
		    {REST, QUARTER}, {NOTE_G7, EIGHTH}, {NOTE_FS7, EIGHTH}, {NOTE_F7, EIGHTH},
		    {NOTE_DS7, QUARTER}, {NOTE_E7, EIGHTH}, {REST, EIGHTH}, {NOTE_GS6, EIGHTH},
		    {NOTE_A6, EIGHTH}, {NOTE_C7, EIGHTH}, {REST, EIGHTH}, {NOTE_A6, EIGHTH},
		    {NOTE_C7, EIGHTH}, {NOTE_D7, EIGHTH}
};



const int start_theme_length = sizeof(start_theme) / sizeof(start_theme[0]);

/* USER CODE END PV */

```

```c
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief 특정 주파수와 지속시간으로 톤 재생
 * @param frequency: 재생할 주파수 (Hz), 0이면 무음
 * @param duration: 재생 시간 (밀리초)
 */
void play_tone(uint16_t frequency, uint16_t duration) {
    if (frequency == 0) {
        // 무음 처리
        HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    } else {
        // 주파수에 따른 ARR 값 계산
        // APB2 클록이 64MHz이고, Prescaler가 64-1이면 1MHz
        // ARR = 1000000 / frequency - 1
        uint32_t arr_value = 1000000 / frequency - 1;

        // 타이머 설정 업데이트
        __HAL_TIM_SET_AUTORELOAD(&htim1, arr_value);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, arr_value / 10); // 50% duty cycle

        // PWM 시작
        HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    }

    // 지정된 시간만큼 대기
    HAL_Delay(duration);

    // 톤 정지
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);

    // 음표 사이의 짧은 간격 (더 빠른 연주를 위해 단축)
    HAL_Delay(30);
}

/**
 * @brief 마리오 테마 음악 재생
 */
void play_start_theme(void) {
    for (int i = 0; i < start_theme_length; i++) {
        play_tone(start_theme[i].frequency, start_theme[i].duration);
    }
}
/* USER CODE END 0 */
```
=> 음 길이에 맞춰 HAL_Delay(duration) + 곡 Note가 다 돌아갈때까지 play_tone()함수 들어감. : piezo 작동동안 조작 불가능
```c
while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

     // 마리오 테마 음악 재생

	  if(start_music_played == 0)
	  {
		  play_start_theme();
		  start_music_played = 1;
	  }

        // 음악 종료 후 5초 대기
        HAL_Delay(1);
         /* USER CODE END WHILE */

  }
  /* USER CODE END 3 */
```
=> 한번만 울리도록
