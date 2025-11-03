# Project1: Smart Car for Drive Test System
(Duration: **25.10.13 - 25.10.31**)
- 01: 프로젝트 목표
- 02: 개발 환경
- 03: System Structure
- 04: 동작 구현 및 설명
- 05: 시현
____
## 01: 프로젝트 목표

### <Project 목표 target><br>
1. 장내 운전면허 시험용 차량
2. 운전 시뮬레이터 학원 연습 장치

### <Project 구현 목표><br>
- **운전 면허 시험코스 축소 구현** : 출발, 직진/ 좌/우회전, ㄷ자코스
- **STM 32 기반 RC카 제어 시스템 구현**
  1. 블루투스 통신(uart3)을 통해 원격 제어 및 모니터링
  2. 적외선 센서를 활용해 주행 상황 인식
- **코스 주행 기능 구현**
  1. 기본 주행 및 정지 제어: 속도 조절 가능
  2. 적외선 센서: event 처리 및 차선 이탈 감지
  3. 감점: 실제 시험 환경에서 발생할 수 있는 감점 요소들을 시험을 구성하여 감점 => Fail/Pass 구분
      (긴급정지, 가속 구간, 라인이탈, 시동)
- **test log를 통해 시험 Fail/ Pass 판단**

  _____
  ## 02: 개발 환경
<img width="939" height="492" alt="image" src="https://github.com/user-attachments/assets/0d85dc9b-022f-41a4-8ba8-ebded7d17c22" /><br>
### <Bom list><br>

<table>
    <td>
<img width="548" height="649" alt="image" src="https://github.com/user-attachments/assets/c06bc575-be07-45fd-950b-d16dff1a7b88" />
    </td>
    <td>

| No. | Name | Description | Qty |
|:---:|:------|:-------------|:---:|
| 1 | STM32 NUCLEO-F103RB | MCU Board | 1 |
| 2 | RC Car Kit | Base Frame | 1 |
| 3 | KY-033 | Tracking Sensor | 2 |
| 4 | MHDND | Piezo | 1 |
| 5 | CSR_BC41C | Bluetooth Module | 1 |
| 6 | ST7735S | LCD | 1 |
| 7 | FTDI232 | Serial | 1 |

</table>

### <MCU><br>
<img width="1158" height="555" alt="image" src="https://github.com/user-attachments/assets/534f6330-0d9b-4a36-9ad2-232313afa7f7" /><br>
___
  ## 03: Project Structure

### <Pin out><br>
<img width="600" height="600" alt="image" src="https://github.com/user-attachments/assets/fa3e2a67-e5cc-4fae-8682-66495a21bfb7" /><br>
<table>
  <tr>
    <td>

<!-- ✅ LEFT TABLE -->
  
| 구분 | Pin | Mode | Label |
|:--|:--|:--|:--|
| **LED** | PA0 | GPIO_Output | LD2 |
| **LCD : SPI1** | PA5 | SPI1_SCK | SPI1_SCK |
|  | PA7 | SPI1_MOSI | SPI1_MOSI |
|  | PA1 | GPIO_Output | LCD_RES |
|  | PA6 | GPIO_Output | LCD_DC |
|  | PB6 | GPIO_Output | LCD_CS |
| **USART1 : USB 통신** | PA2 | USART_TX | USART_TX |
|  | PA3 | USART_RX | USART_RX |
| **USART3 : Bluetooth** | PC10 | USART3_TX | USART3_TX |
|  | PC11 | USART3_RX | USART3_RX |
| **PIEZO** | PA8 | TIM1_CH1 | piezo |

  </td>
  <td>

<!-- ✅ RIGHT TABLE -->

| 구분 | Pin | Mode | Label |
|:--|:--|:--|:--|
| **DC 모터** | PA9 | TIM1_CH2 | LRF |
|  | PA10 | TIM2_CH2 | LFB |
|  | PB3 | TIM1_CH3 | LFF |
|  | PB10 | TIM2_CH3 | LRB |
|  | PB4 | TIM3_CH1 | RFF |
|  | PB5 | TIM3_CH2 | RFB |
|  | PB8 | TIM4_CH3 | RRF |
|  | PB9 | TIM4_CH4 | RRB |
| **Line Sensor** | PB13 | GPIO_Input | L_Lline |
|  | PC8 | GPIO_Input | R_Rline |
| **Random 변수** | PC4 | ADC11_IN16 | Random |

  </td>
  </tr>
</table>

### <HW System Strecture><br>
<img width="1695" height="968" alt="image" src="https://github.com/user-attachments/assets/e1df9195-a7ff-40c8-b06e-3cb735c61412" /><br>
### <SW System Strecture><br>
<img width="1630" height="948" alt="image" src="https://github.com/user-attachments/assets/922ce54f-ada9-4cd6-9296-0ebf7e3573d0" /><br>

  ## 04: 동작 설명 및 구현
  ## Overview
<img width="1663" height="863" alt="image" src="https://github.com/user-attachments/assets/8decd476-d55e-486f-8f70-ca97754772e4" /><br>

### 1) Bluetooth
: uart3(11520bps) 사용 <br>

| 장치                | 연결 방식            | 역할                          |
| ----------------- | ---------------- | --------------------------- |
| 노트북 (Tera Term)   | Bluetooth 무선 통신  | 시험 관리자 입력 / 데이터 모니터링        |
| BT 모듈 (CSR_BC41C) | UART3            | MCU ↔ PC 데이터 송수신            |
| MCU (STM32)       | UART3, GPIO, PWM | 차량 제어, 센서 읽기, LCD 출력, 점수 계산 |<br>

- 차량 제어
- test 환경 제어
  <img width="1698" height="963" alt="image" src="https://github.com/user-attachments/assets/bef3e9f2-69ef-42b6-a194-d2e77e714758" /><br>
<차량 제어: Bluetooth -> MCU -> 모터> <br>
  
| 기능        | 입력 키                           | 설명                                 |
| --------- | ------------------------------ | ---------------------------------- |
| **이동 제어** | `w`(↑), `s`(↓), `a`(←), `d`(→) | RC 카 이동 방향 제어                      |
| **시동 제어** | `o` (토글)                       | ON: 주행 시작 시간 기록 / OFF: 주행 종료 시간 기록 |
| **속도 제어** | `h`(고속), `j`(중속), `k`(저속)      | DC 모터 PWM 속도 단계 제어                 |
| **비상 정지** | `u`                            | 비상 상황 시 즉시 정지 (피에조 알람 포함)          |<br>

<Test 환경 제어><br>  
  | 항목              | 내용                                                                   |
| --------------- | -------------------------------------------------------------------- |
| **1) Mode 설정**  | - Test Mode: `1` 입력 시 시험 시작 부저 동작 <br> - Practice Mode: 입력 즉시 시동 `o` |
| **2) Event 제어** | 랜덤 이벤트 구역 설정, 진입 시 부저 작동                                             |
| **3) 감점 로직**    | 라인 이탈, 속도 위반, 이벤트 미대응 등 조건 처리                                        |
| **4) 실시간 출력**   | mode 상태, on/off 여부, 감점 요소, 속도                                        |
| **5) 주행 종료 출력** | 총 주행시간, line 이탈 횟수, 최종 점수 <br>(LCD: 운전자 / TeraTerm: 시험 관리자용 출력)      |<br>

### 2) 시동 기능/ mode 설정
<img width="1693" height="897" alt="image" src="https://github.com/user-attachments/assets/2749aa0e-d86b-403c-8a72-b12886a54358" /><br>
- 시동 OFF: 차량 제어 불가능
- 시동 ON: 차량 제어 가능
- Mode: Test/ Practice 구분


### 3) 차선 감지 & 4) event 구간
- 차선 감지: 오른쪽/ 왼쪽 라인 이탈 감지 & event 구간 감지<br>
- event 구간: 주행중 가속구간 동작 주행 중 랜덤 정지 구간 동작<br>
  <축소 구현한 운전 면허 시험 simulation 코스><br>
  <img width="600" height="600" alt="image" src="https://github.com/user-attachments/assets/8857a7e6-7838-4f1a-be75-40de6d4f53dc" /><br>
  **목표 1) 왼쪽, 오른쪽 차선 이탈 감지**
  - 주행 내 발생한 이탈 횟수 count(-5점)<br>
  **목표 2) 3가지 구간 중 랜덤으로 긴급 정지 event 실행**
  - Pass: 부저가 울리고 3초내 'u' (비상 정지) 입력<br>
  - Fail: 3초 초과시 timeover로 감정(-10점)
 
  **목표 3) 4번째 구간에서 가속 event 실행**
  - Pass: 부저가 울리고 3초내 'h'(속도 100%)입력
  - Fail: 3초 초과시 timeover로 감점(-10점) 
<Inital Plan><br>
<img width="1146" height="681" alt="image" src="https://github.com/user-attachments/assets/d40a5582-e409-4a7c-825b-bb32cfd4d71f" /><br>
<img width="1143" height="583" alt="image" src="https://github.com/user-attachments/assets/4516a011-2068-480f-bd0b-4ff80b1c3c95" /><br>
=>  목표 2,3은 성공했지만 **목표 1 실패**<br>
##### <목표1: 왼쪽 오른쪽 차선 이탈 감지 X>
<img width="1024" height="344" alt="image" src="https://github.com/user-attachments/assets/61480953-2acc-4ace-bffb-13a717e78520" /><br>

- 이유 1) 왼쪽, 오른쪽 자외선 센서 동기화 X
         => Both on Black State가 될때 R/L 중 한개가 먼저 읽이면서 라인 이탈로 카운트<br>
- 이유 2) 라인 이탈 후, 다시 차선으로 돌아오는 과정에서 라인 이탈 카운트가 한번 더됨<br>

**<추가 보완 1: Both on Black state 전 한쪽 센서가 먼저 반응에서 이탈로 잡는 오류 방지>**<br>
<img width="735" height="542" alt="image" src="https://github.com/user-attachments/assets/a32e26ec-6e7f-4585-bc9c-aab0e7245456" /><br>
- Delay로 센서 값 저장 후, 한번 더 센서 값 측정
  => R/L 여전히 조건과 일치 하면 state 확정
  => Both on Black State: 센서 값을 측정해서 Both on Black State라면 Both on Black State로 확정 <br>
**<추가 보완 2: 라인 이탈 시 선을 넘었을 경우, 다시 들어올대 line over 중복 카운트 해결>**<br>
<img width="1211" height="577" alt="image" src="https://github.com/user-attachments/assets/983d1b2a-af98-497d-994d-d7654d9e9c26" /><br>

- **오류3**: 키 입력 없이 키의 딜레이 값에 의해 모터가 작동했을경우, 그동한 state가 바뀌었다면 그 전 state 상태로 바꿔줌
- **오류2**: 차가 나가는 동안 입력되는 키 가 같고, 차가 들어 올 때 입력되는 키가 동일하다고 판단
   => change 변수를 추가<br>
   => 나갈때 일어나는 두번의 상태 변화, 즉 w->b(ch1) b->w(ch2)일 때 ch1==ch2이면 change++<br>
   => 들어올때 일어나는 두번의 상태 변화, 즉  즉 w->b(ch3) b->w(ch4)일 때 ch3==ch4이면 change++<br>
   => change=2가 되면 lineover 카운트를 1개 제거해서 중복 카운트 방지<br>
    

### 5) PWM 조절
- 속도 3단계 조절(LOW/Middoe/High)
- Piezo
##### PWM 조절1: 속도 조절절

<table>
  <tr>
    <td>
<!-- ✅ LEFT TABLE -->
      
  | 구분        | 동작 | 타이머 / 채널 | Label |
| --------- | -- | -------- | ----- |
| **좌측 모터** | 전진 | TIM1 CH2 | LRF   |
|           | 전진 | TIM2 CH2 | LFF   |
|           | 후진 | TIM1 CH3 | LFB   |
|           | 후진 | TIM2 CH3 | LRB   |
| **우측 모터** | 전진 | TIM4 CH3 | RRF   |
|           | 전진 | TIM3 CH1 | RFF   |
|           | 후진 | TIM4 CH4 | RRB   |
|           | 후진 | TIM3 CH2 | RFB   |
      
  </td>
  <td>
<!-- ✅ RIGHT TABLE -->
<img width="725" height="480" alt="image" src="https://github.com/user-attachments/assets/a95e943b-03a1-4ded-8875-00e4de5c5715" />
  </td>
  </tr>
</table>

<table>
  <tr>
    <td>
<!-- ✅ LEFT TABLE -->
<img width="482" height="314" alt="image" src="https://github.com/user-attachments/assets/257cf8d2-1acd-4735-b051-099082bfc0d8" />

  </td>
  <td>
<!-- ✅ RIGHT TABLE -->
          * PWN 기본 설정(주파수 고정, 듀티비만 가변)<br>
      - ARR= 999 고정
        : 약 1khz (PSC:63=> 64M/64/(999+1)= 1k)<br>
    
      *  속도 변수, spd(0-1000) => duty(%)= spd/(ARR+1) *100)
    
  </td>
  </tr>
</table>

##### PWM 조절2: PIezo 제어
: PWM으로 **"음 높이"(주파수)** => ARR(주기 설정)
- 타이머 주기형성 1)PSC(프리스케일러) 2) ARR(오토리로드) => **ARR로 조절**
- 출력 주파수 <br>
<img width="322" height="78" alt="image" src="https://github.com/user-attachments/assets/944d4620-13c7-4e83-abf4-5c4b09002af4" /><br>

< PWN 기본 설정(주파수 고정, 듀티비만 가변)><br>
- PSC= 64-1 : 타이머 clk: 1MHz
- ARR= 1,000,000/f -1로 계산 (음표 주파수에 맞추기)
- CCR: duty rate 결정(소리 크키/ 파형 다듬기)
    => 보통 50%(CRR=ARR/2)에서 가장 깨끗하고 큰 소리 남( 듀티를 너무 낮추면 작아지고 키우면 왜곡 커짐)<br>
    <img width="974" height="577" alt="image" src="https://github.com/user-attachments/assets/ea533d05-6db0-4bd9-bf7c-6dbbfa909ac6" />
**<추가 보완 1: TIM 리소스 제한으로 buzzer 사용 중 PWM 모터 제어 불가 => "Non Blocking으로 해결">**<br>
- 원인: 바퀴 모터와 타이머 공유 (TIM1)
      => 모터 제어(CH2, CJ3): ARR=999(고정)/ CRR= 0-1000 duty(속도제어)<br>
      =>


          
















(2)모터 제어 함수 (방향+ 속도)<br>
```c
void smartcar_forward(uint16_t spd) {
     // forward핀만 ON
     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, spd); // LRF
     __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, spd); // LFF
     __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, spd); // RRF
     __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, spd); // RFF

     // back핀은 0
     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0); // LFB
     __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0); // LRB
     __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0); // RFB
     __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0); // RRB
   //__HAL_TIM_SET_COMPARE(&htimx, TIM_CHANNEL_y, CCR_Value) -> duty(80): CCR 800, duty(50): 500 ,->duty(100)
   //ARR 값이 1000-1 이기 때문에


}

void smartcar_backward(uint16_t spd) {
     // backward핀만 ON
     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, spd); // LFB
     __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, spd); // LRB
     __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, spd); // RFB
     __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, spd); // RRB

     // forward핀은 0
     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0); // LRF
     __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0); // LFF
     __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 0); // RRF
     __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0); // RFF
}

void smartcar_stop(void) {

     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0); // LFB
     __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0); // LRB
     __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0); // RFB
     __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0); // RRB


     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0); // LRF
     __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0); // LFF
     __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 0); // RRF
     __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0); // RFF
}

void smartcar_left(uint16_t spd) {
     // 왼쪽 느리게(또는 후진), 오른쪽 전진 — 취향대로
     // 여기선 왼쪽 후진, 오른쪽 전진
     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, spd); // LFB
     __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, spd); // LRB
     __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, spd); // RFF
     __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, spd); // RRF

     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
     __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
     __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
     __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
}

void smartcar_right(uint16_t spd) {
     // 오른쪽 후진, 왼쪽 전진
     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, spd); // LRF
     __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, spd); // LFF
     __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, spd); // RFB
     __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, spd); // RRB

     __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
     __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
     __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
     __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 0);
}
```
3)  w&s&a&d(방향키)/ on&off/ 1(test mode) 부저 알림/ h&j&k(speed 조절): **uart3 인터럽트 처리**
```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
   last_time = HAL_GetTick();// 키 입력 될때 마다 시간 측정
   if (huart == &huart3)
   {
      if (ch == '1') // 1번 눌렀을 때 부저
      {
        power = 0;
         state = 1;
         past_state = 1;// Practice  mode &Test mode 구분
         buzzer_start_time = HAL_GetTick();
         LCD_Fill(BLACK);
      }

      else if (past_state == 2 && ch == 'o')// 시동 토글(토글 조건은 항상 앞에)
      {
         power = !power;
         HAL_GPIO_WritePin(GPIOA, LD2_Pin, 0);
         state = 0;
         past_state=0;
         t_end = HAL_GetTick();
         duration_ms = (t_end - t_start);
         smartcar_stop();
      }

      else if (ch == 'o') // 5초 안에 아직 시동 까지만
      {
         printed=0;
         LCD_Fill(BLACK);
          LCD_DrawString(10, 60, "speed: low     ", WHITE, BLACK); //기본 속도

         if(past_state!=1 || state==3) // 시험 피에조가 울리지 않거나 시동못걸고 탈락 후 바로 시동: 연습모드
         {
            LCD_DrawString(10, 45, "Practice Mode", WHITE, BLACK);
         }
         else
         {
            LCD_DrawString(10, 45, "Test Mode     ", WHITE, BLACK);
         }
        state = 2;// 시동
        past_state=2;
         power = 1;
         spd=300;// 기본 속도

         t_start = HAL_GetTick();// 시동 걸린 시간 측정
      }


//속도 조절
      else if(ch=='h')
      {
           spd=1000;//MAX
           LCD_DrawString(10, 60, "speed:High     ", WHITE, BLACK);
        }

        else if(ch=='j')
        {
           spd=500;
           LCD_DrawString(10, 60, "speed: middle    ", WHITE, BLACK);
        }

        else if (ch=='k')
        {
           spd=300;
           LCD_DrawString(10, 60, "speed: low     ", WHITE, BLACK);
        }
      HAL_UART_Receive_IT(&huart3, &ch, 1); // 다음 입력 등록
   }
}
```
**main 함수는 마지막에 최종적으로 추가**




  












