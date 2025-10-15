# 3) EXIT
기능) B1 버튼을 누름 -> LED가 켜져있으면 끄고, 켜져 있으면 켠다: Toggle<br>
![EXIT](https://github.com/user-attachments/assets/9d39c5b6-b0d3-4046-b6e5-d343873ea304)

### (1) Pinout & Configuration
#### 01) RCC(Reset & Clcok Contorl)
<img width="1105" height="552" alt="image" src="https://github.com/user-attachments/assets/fea9eacc-5b89-47b8-8bb4-f71d4eb84576" /><br>
#### 02) interrpt 사용 시
- 버튼을 입력 핀으로 설정하고 해당 핀에 인터럽트 기능을 활성화<br>
<img width="926" height="670" alt="image" src="https://github.com/user-attachments/assets/2d2ea926-5483-45f9-b841-2c92d7576373" /><br>
<img width="1538" height="654" alt="image" src="https://github.com/user-attachments/assets/807db4b7-a69a-4b80-a04d-a8c236d1fa47" />


### (2) 코드작성
- main.c 추가 코드

```c
//버튼(외부 인터럽트)-> 콜백함수 실행 => 핀번호 확인(B1_Pin(13) -> LED 토글(누르면 켜지다가 다음번누를때 꺼짐)
void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
	case B1_Pin://B1을 눌렀을 때
		HAL_GPIO_TogglePin (LD2_GPIO_Port, LD2_Pin);// LED가 켜져잇으면 끄고, 켜져 있으면 켠다: Toggle
		break;

	default:// 그 외 버튼은 무
		;
	}
}
```
=>B1으로 LED Toggle
