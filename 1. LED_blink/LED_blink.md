# 1) LED_blink

![1 LED_blink (1)](https://github.com/user-attachments/assets/e4734f20-71ef-40fb-95f9-09226390d6c6)

### (1) New Project 생성
<img width="948" height="594" alt="image" src="https://github.com/user-attachments/assets/9724186d-c4cc-495c-85a7-6df966a36b53" />
<img width="1352" height="746" alt="image" src="https://github.com/user-attachments/assets/93374bde-71a5-442c-9e11-282a32213709" />
<img width="1039" height="562" alt="image" src="https://github.com/user-attachments/assets/e72fc7ad-a1b5-4ddc-b2f9-e4347d5e9161" />
<img width="1184" height="634" alt="image" src="https://github.com/user-attachments/assets/f9ba7a42-d2d0-43be-8c3f-d9f1acfced2e" />

### (2) Pinout &Configuration

#### 01) RCC(Reset & Clcok Contorl)
- Nucleo -F103RB에서 사용가능한 클록소스는 크게 내부클럭, 외부클럭이 있다.

(외부클럭: ST-Link의 8MHz 크리스탈 출력을 bypass해서 사용. X3에 8MHz 크리스털을 납땜하여 그 출력을 클럭으로 사용)
<img width="400" height="500" alt="image" src="https://github.com/user-attachments/assets/b2bfdf3d-d773-4415-8296-dbd1ba25e5a3" />

- 앞으로 실습에선 **내부 클럭**을 사용   
<img width="1481" height="863" alt="image" src="https://github.com/user-attachments/assets/2f324a48-c783-4782-864c-9882347ad67c" />
<img width="2412" height="894" alt="image" src="https://github.com/user-attachments/assets/556c5bb1-9724-4064-a123-23d332a8e74d" /><br><br>

***RCC 설정: 클럭소스로 내부RC 클럭 발진회로 선택***
<img width="1105" height="552" alt="image" src="https://github.com/user-attachments/assets/fea9eacc-5b89-47b8-8bb4-f71d4eb84576" />

- Clock Configuration 탭에서 SYSCLK이 64(MHz)로 설정 확인
<img width="1054" height="584" alt="image" src="https://github.com/user-attachments/assets/18e5eeee-bd40-47ac-8904-44e1eeaf5e29" />

#### 02) TEST PIN 설정
- 디버깅이 필요할 때 등, 테스트를 위한 핀 부여
- 오른쪽 마우스: GPIO_Output/ 왼쪽마우스: Enter User Label<br>
<img width="433" height="413" alt="image" src="https://github.com/user-attachments/assets/77d6e7a2-ad59-4bab-85e9-c1c6dfd07244" />



### (3) 코드 생성
- alt + k
<img width="545" height="425" alt="image" src="https://github.com/user-attachments/assets/fbec1e1d-366f-423e-ba6f-5afdcd8a9aac" /><br>
- USER CODE BEGIN ~ USER CODE END사이에 코드를 작성해야지 코드가 지워지지 않음

### (4) 코드 작성 
- main 함수내의 infinite loop 일부 수정
  <img width="236" height="312" alt="image" src="https://github.com/user-attachments/assets/a8c64998-ca5d-4e45-a300-bb98e742b303" /><br>


* main.c
```c
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);//=HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1); 동일
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
	  HAL_Delay(1000);//1초 동안 켜진상태
	  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);// 1초동안 꺼진 상태
	  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
	  HAL_Delay(1000);
//LED blink 주기 2초
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
```
### (4) Build Project & Upload
<img width="1401" height="698" alt="image" src="https://github.com/user-attachments/assets/47967671-5fc8-4c08-b41e-c93ee9f2cc20" /><br>
- Build & RUN까지 완료되면 타겟보드에서 녹색 LED가 1초 간격으로 깜빡거리는 것을 확인할 수 있다.
- 만약 Build 과정에서 error가 발생한다면 Debug를 통해 error를 수정하면 된다

### (5) Debugging
<img width="1493" height="462" alt="image" src="https://github.com/user-attachments/assets/6093513e-f506-4c90-802d-2b2f4b9ad60f" /><br>
- Debug 모드에 들어가면 초록색 세모 버튼을 누르면 핀을 찍은 부분(104번)까지 코드가 돌아감
- 노란 화살표를 누르면 한개씩 코드가 진행: error가 없다면 다음 코드로 넘어가지고 error 발생시 넘어가지지 않음.



  












