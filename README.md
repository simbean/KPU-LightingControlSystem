# README.md

# smart lighting control system using exterior illumination sensor

***

### Overview
![image](https://user-images.githubusercontent.com/71861842/147445650-4e166763-1dd7-485a-b27a-cb2b411f0664.png)

한국산업기술대학교 졸업 작품

## Structure

***

- 전체 구상도

![image](https://user-images.githubusercontent.com/71861842/147445560-ea64f3d1-6dc0-4c96-961e-7218d2cdddf9.png)

- hardware 부품 구상도

![image](https://user-images.githubusercontent.com/71861842/147445573-35c53f99-94bf-4356-988c-ab6cf057f827.png)

## SKILLS

***

- android
- Arduino
- Bluetooth

## Android

***

- Android 화면 구성
<br>
![image](https://user-images.githubusercontent.com/71861842/147445634-052f4124-730b-4a40-9602-199e2887350d.png)

## 주요 기능

어플리케이션을 통한 실내 조도값측정, 그에 따라 사용자가 원하는 모드(실내 조도)를 입력받아 커튼과 조명이 적절하게 작동하도록 한다.
사용자가 설정할 수 있는 자동 모드는 총 4가지로 아래와 같다.

1. 실내 조도 값을 기준으로 한 밝기에 따른 4가지의 모드
(1) Cooling	집중 모드 / 기준 lux = 1000 lux
(2) Care	편안 모드 / 기준 lux = 700 lux
(3) Healing	안락 모드 / 기준 lux = 150 lux
(4) Sleep	수면 모드 / 기준 lux = 0 lux
2. 시간 설정기능
- 4가지의 모드로 설정되어 동작하던 중 원하는 시간( 출근시간, 퇴근시간, 수면시간) 등에 맞추어 다른 모드로 바꾸어주는 기능이다.
    - 기상 시 기상 시간을 설정하여 sleep모드에서 healing모드 또는 cooling모드로 전환. 퇴근시간이나 잠자는 시간을 설정하여 cooling모드에서 sleep모드로 전환.
    기상시간 아침 9시 30분 sleep mode -> cooling mode
    퇴근시간 오후 6시 cooling mode -> sleep mode
1. 수동조작모드
- 기본 커튼의 기능을 이용하고 싶을 때 (원할 때 자유롭게 올리고 내리는 것) 사용하는 것으로 자동모드보다 우선적인 기능을 가지고 있어 수동모드가 동작될 때는 자동모드가 해제 된다


