hu#include <Wire.h>
#include <BH1750.h>
#include <SoftwareSerial.h>
#define CONTINUOUS_HIGH_RES_MODE 0x10  // Measurement at 1 lux resolution. Measurement time is approx 120ms
#define BH1750_1_ADDRESS 0x23  // Sensor 1 connected to GND
#define BH1750_2_ADDRESS 0x5C

///핀설정
#define led 11
#define led_warm 6
#define ENA 9
#define IN1 10
#define IN2 12
#define OUTDOOR 500     //외부조도
#define PWM 40            //LED 값
#define delaymotor 10000  //모터 딜레이값
SoftwareSerial BT_Serial(7,8);//TX,RX
BH1750 lightMeter;

int16_t RawData;
int16_t SensorValue[2];

int OnOff = 0; //수동모드
bool On = false;
int val;
int val2;
int Lfin = 0; // 세분화 단곗값
int B_lev = 0; // 블라인드 단계조절
int L_lev = 0; // led 단계조절
char a = 'S'; //모드 S=sleep,C= cool,R  = care , H = healing
int Time = 0; //시간모드
int Value = 0;
static char colorTemp = 'L';
void setup() { 
  BT_Serial.begin(9600);// 블루투스 시리얼
  Wire.begin();
  lightMeter.begin();
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  // 모터 출력핀
  pinMode(led, OUTPUT); // led 출력핀
  pinMode(led_warm, OUTPUT);// LED 6500K
  Serial.begin(9600); 

}

void MOTER_STOP()
{  
    digitalWrite(ENA, LOW);
}
void error() {
     //Serial.println("에러");
}

void MOTER_LEFT()//한단계 올림
{   
   if(B_lev > 3){
    MOTER_STOP();
  }
  else{
    analogWrite(ENA, 200);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    delay(3800);
    MOTER_STOP();
  }
}

void MOTER_RIGHT() //한단계 내림
{ 
  if(B_lev <0){
      MOTER_STOP();
  }
  else{
    analogWrite(ENA, 200);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    delay(3800);
    MOTER_STOP();
  }
 
}

void led_lev(int lev)
{
    if(a == 'C') {
      if((PWM*lev)<256){
       // Serial.print((PWM*lev));
          analogWrite(led, (PWM*lev)); 
          analogWrite(led_warm, LOW);
      }
    }
    else if(a == 'H' | a == 'R'){
      if((PWM*lev)<256){
       // Serial.print((PWM*lev));
          analogWrite(led_warm, (PWM*lev)); 
          analogWrite(led, LOW);

      }
    }
}
void led_off()
{
    L_lev = 0;
    digitalWrite(led, LOW);
    digitalWrite(led_warm, LOW);
    //Serial.println("조명을 0단계로 변경합니다.");
}

int light(int stage) { //stage 1 = 단계올리기 -1 = 단계 내리기
    L_lev = L_lev + stage;
    Lfin = (255/PWM)+1; //단계 최댓값
    if(L_lev<0){
      L_lev = 0;
    }
    if (stage == 1) {
     // Serial.println("빛 단계를 올립니다.");
      led_lev(L_lev);
    }
    else if(stage == -1){
      //Serial.println("빛 단계를 내립니다.");
      led_lev(L_lev);  
    }
    else{
      error();
    }
    if (L_lev <= Lfin) {
 //   Serial.print("LED조명 최대 단계는 ");
 //   Serial.print(Lfin);
 //   Serial.println("단계 입니다.");
    }
    else if(L_lev > Lfin ) {
    L_lev = Lfin;
    //Serial.println("최대 단계는 7단계 입니다. ");
    //Serial.print(Lfin);
    //Serial.println("단계를 유지합니다.");
  }
    else L_lev = 0;
    
    //Serial.print("현재 조명 "); Serial.print(L_lev); Serial.println("단계입니다.");

    return L_lev;
}



int blind(int stage) { //stage 1 = 단계올리기 -1 = 단계 내리기
    B_lev = B_lev + stage;
      //Serial.print(B_lev);
    if (stage == 1) {
      //Serial.print(B_lev);
      if(B_lev>3){
        light(1);
      }
      else{
      //Serial.println("블라인드를 올립니다.");
      MOTER_LEFT();
      }
    }
    else if(stage == -1){
      //Serial.print(B_lev);
      if(B_lev >= 3){
        light(-1);
      }
      else{
      //Serial.println("블라인드를 내립니다.");
      MOTER_RIGHT();
      }
    }
    
    if (B_lev > 0 && B_lev <= 10) {
            //Serial.print("블라인드 단계를"); 
            //Serial.print(B_lev); 
            //Serial.println("단계로 변경합니다."); // 블라인드 끝까지 내림
    }
    else if (B_lev > 10) {
       // Serial.println("더이상 조도를 올릴 수 없습니다. 3단계를 유지합니다");
        B_lev = 10;
    }
    else B_lev = 0;
    
    //Serial.print("현재 블라인드 ");
    //Serial.print(B_lev);
   // Serial.println("단계입니다");
    
    return B_lev;
}

int stable(char mode) {
    if (mode == 'C') {
        //Serial.print("Cooling");
    }
    else if (mode == 'R') {
       // Serial.print("Care");
    }
    else if (mode == 'H') {
      //  Serial.print("Healing");
    }
    else if (mode == 'S') {
       // Serial.print("Sleep");
    }
    else error();
    //Serial.println("모드이고, 빛을 유지합니다 ");
    //delay(5000);
    
    return mode;
}

int Distinguish(char mode, int val, int val2) {
    if (mode == 'C') {
      if (val >= 1100) {
            //Serial.print("조도가 1000Lux이상이므로 ");
            if (val2 >= OUTDOOR) {
              blind(-1);
            }
            else
            {
              light(-1);
            }
      }
      else if (val >= 900)  stable(mode);
      else {
           // Serial.print("Cooling모드, 조도가 ");
            //Serial.print(val);
            //Serial.print(" Lux 이므로 ");
            if (val2 >= OUTDOOR) blind(1);
            else light(1);
      }
    }
    else if (mode == 'R') {
      if (val >= 800) {
            //Serial.print("조도가 700 Lux이상이므로 ");
          if (val2 >= OUTDOOR)  blind(-1);
            else light(-1);
      }
      else if (val >= 600) stable(mode); 
      else {
            //Serial.print("Care모드, 조도가 ");
            //Serial.print(val);
            //Serial.print(" Lux 이므로 ");
            if (val2 >= OUTDOOR) blind(1);
            else light(1);
      }
    }
    else if (mode == 'H') {
      if (val >= 250) {
           // Serial.print("조도가 150 Lux이상이므로 ");
           if (val2 >= OUTDOOR) blind(-1);
           else light(-1);
      }
      else if (val >= 50) stable(mode);
      else {
           //Serial.print("Healing모드, 조도가 ");
            //Serial.print(val);
            //Serial.print(" Lux 이므로 ");
            if (val2 >= OUTDOOR) blind(1);
            else light(1);
      }
    }
    else if (mode == 'S'){
        led_off();
        blind(-1);
    }
    else error();

    return mode;
}
void SelectMode(int val){
    static int previousmotor = 0;  //모터의 작동 원리는 딜레이에 맞추어 올리고 내리고 하는 것이기 때문에 led처럼 analog못줌
    static int previouslight = 0;
    static char previousmode = 'L';
    int total = 0;
    previouslight = L_lev;
    previousmotor = B_lev;
    previousmode = colorTemp;
    //Serial.println("수동기능 on");
            if(val == 48){
              B_lev = 0;
            }
            else if(val == 49){
              B_lev = 1;
            }
            else if(val == 50){
              B_lev = 2;
            }
            else if(val == 51){
              B_lev = 3;
            }
             else if(val == 52){
              L_lev = 0;
            }
            else if(val == 53){
              L_lev = 2;
            }
            else if(val == 54){
              L_lev = 4;
            }
             else if(val == 55){
              L_lev = 6;
            }
            else if(val == 67 || val == 72 || val == 82 || val == 83){
              OnOff = 0;
              if(val2>OUTDOOR){
                L_lev = 0;
              }
              else{
                B_lev = 0;
              }
            }
            else if(val == 76) colorTemp = 'L';
            else if(val == 87) colorTemp = 'W';
            //Serial.print(previouslight);
            //Serial.println(previousmotor);
            //Serial.print(L_lev);
            //Serial.println(" 단계(조명)");
            //Serial.print(B_lev);
            //Serial.println(" 단계(블라인드)");
            if(!(previouslight== L_lev && previousmode == colorTemp)){
              if(colorTemp == 'L'){
                analogWrite(led, (PWM*(L_lev))); 
                analogWrite(led_warm,0);
              }
              else if(colorTemp == 'W'){
                analogWrite(led_warm, (PWM*(L_lev))); 
                analogWrite(led,0);
              }
              //Serial.println("조명 조절 완료");
            }
            if(previousmotor>3){
              previousmotor = 3;
            }
              if(!(previousmotor== B_lev)){
              total = previousmotor - B_lev;
               if(total>0){ //원래값 3 , 변경값 2 => 블라인드 내려야함
                for(int i=0;i<total;i++){
                  //Serial.print(total);
                   MOTER_RIGHT();//내림
                }
              }
               else if(total<0){ //원래값 2 , 변경값 3 => 블라인드 올려야함
                //Serial.print(0-total);
                for(int i=0;i<(0-total);i++){
                   MOTER_LEFT();//올림
                }
              }
              }       
           
}

void loop() {//조명 열거나 커튼 열기 loop()에 들어갈거
        char s1[30];
        int flag = 0;
        init_BH1750(BH1750_1_ADDRESS, CONTINUOUS_HIGH_RES_MODE);//실내
        delay(500);
        RawData_BH1750(BH1750_1_ADDRESS);
        SensorValue[0] = RawData / 1.2;  

        init_BH1750(BH1750_2_ADDRESS, CONTINUOUS_HIGH_RES_MODE);//외부
        delay(500);
        RawData_BH1750(BH1750_2_ADDRESS);
        SensorValue[1] = RawData / 1.2;
        
        //Serial.println("------------------------------------------------------------");
        //Serial.print("< 실내 Sensor_1 = "); Serial.print(SensorValue[0]); Serial.print(" Lux");
        //Serial.print(" | 외부 Sensor_2 = "); Serial.print(SensorValue[1]); Serial.println(" Lux >");
        val = SensorValue[0];
        if(val2 < OUTDOOR){
          if(SensorValue[1]>OUTDOOR){
              flag = 1;
          }
        }
        else if(val2 > OUTDOOR){
          if(SensorValue[1]<OUTDOOR){
              flag = 2;
          }
        }
        val2 = SensorValue[1];
        //Serial.print("현재 모드 = ");
        //Serial.println(a);
        if(BT_Serial.available()){ 
              Value = BT_Serial.read();
            }
        else Value = 0;
            if(Value != 0){
            //Serial.print("BT 정보 받았어요! :");
            //Serial.println(Value);
            }
             if(Value >= 48 && Value<= 55){
                OnOff = 1;
                //Serial.println(OnOff);
              }
             else if(Value == 67){
              a = 'C';
              Serial.println(-1);
             }
             else if(Value == 72){
              a = 'H';
              Serial.println(-2);
             }
             else if(Value == 82){
              a = 'R';
              Serial.println(-3);
             }
             else if(Value == 83){
              a = 'S';
              Serial.println(-4);
             }
             
       sprintf(s1, "a%db%dc%cd%de%df%dn", val, val2, a, B_lev, L_lev, OnOff); 
       Serial.println(val);
       //Serial.println(s1);
       BT_Serial.write(s1);     
       if (OnOff == 1) {
            SelectMode(Value);
        }
        else {
            if (flag == 2)
            {
              while (1){
                blind(-1);
                if(B_lev == 0){
                  flag = 0;
                  break;
                }
              }
            }
            else if (flag == 1){
              while(1){
                light(-1);
                if(L_lev == 0){
                  flag = 0;
                  break;
                }
              }
            }
             Distinguish(a, val, val2); 
        }
        delay(100);
    }


void init_BH1750(int ADDRESS, int MODE){
  //BH1750 Initializing & Reset
  Wire.beginTransmission(ADDRESS);
  Wire.write(MODE);  // PWR_MGMT_1 register
  Wire.endTransmission(true);
}

void RawData_BH1750(int ADDRESS){
  Wire.beginTransmission(ADDRESS);
  Wire.requestFrom(ADDRESS,2,true);  // request a total of 2 registers
  RawData = Wire.read() << 8 | Wire.read();  // Read Raw Data of BH1750
  Wire.endTransmission(true);
}
