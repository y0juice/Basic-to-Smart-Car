# Project1: Smart Car for Drive Test System
(Duration: 
- 01: 프로젝트 목표
- 02: 개발 환경
- 03: System Structure
- 04: 동작 구현 및 설명
- 05: 시현
____
## 01: 프로젝트 목표

#### <Project 목표 target><br>
1. 장내 운전면허 시험용 차량
2. 운전 시뮬레이터 학원 연습 장치

#### <Project 구현 목표><br>
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
#### <Bom list><br>

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

#### <MCU><br>
<img width="1158" height="555" alt="image" src="https://github.com/user-attachments/assets/534f6330-0d9b-4a36-9ad2-232313afa7f7" /><br>




