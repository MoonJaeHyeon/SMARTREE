/* MANUAL 입니다.
  전원은 DC CABLE로 연결 
  BlueTooth  tx=2 , rx=3
  DHT temp & Humi sensor =4
  relay = 8
  LCD  SDA=SDA / SCL=SCL
  SOIL MOISTURE SENSOR = A0
*RELAY & MOTOR 설정
  MOTOR (+) = ARDUINO 5V or BREAD BOARD (+)
  MOTOR (-) = RELAY 4
  RELAY 5 = ARDUINO GND or BREAD BOARD (-)
*/
#include "DHT.h"     // 온도습도를 센서를 위한 DHT.h 라이브러리를 포함한다
#include <SoftwareSerial.h>     //SoftwareSerial을 통해 블루투스와 데이터 전송
#include <LiquidCrystal_I2C.h>    //lcd 작동을 위한 header

#define DHTPIN 4      // DHT핀(온 습도 센서 데이터)을 4번으로 정의한다(DATA핀)
#define DHTTYPE DHT11  // DHT타입을 DHT11로 정의한다
#define Relay 8       // 모터 제어 Relay
#define soilSensor 14     // soil-moisture soilSensor 아날로그 A0-A0 , D0-A1
unsigned int soil;

SoftwareSerial BTSerial(2,3); // (보드에 rx-3 , tx-2)
LiquidCrystal_I2C lcd(0x3F,20,4); // lcd 설정   sda / scl 

DHT dht(DHTPIN, DHTTYPE);  // DHT설정 - dht (디지털4, dht11)

void setup(){
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);    // 9600 속도로 시리얼 통신을 시작한다
  Serial.println("Hello!");
  pinMode(soilSensor,INPUT);
  pinMode(Relay, OUTPUT);
  
  BTSerial.begin(9600);  // 9600 속도로 블루투스시리얼(소프트웨어시리얼) 통신을 시작한다
}

void loop(){
   soil = analogRead(soilSensor);
  
  //블루투스 통신 끝
  float temp = dht.readTemperature();  // 변수 temp에 온도 값을 저장
  float humi = dht.readHumidity();  // 변수 humi에 습도 값을 저장
    delay(2000); // 시작 시 대기시간 줌 (처음 3번 정도 습도 센서 읽기 안됨)
  if (isnan(temp) || isnan(humi)) {
    //값 읽기 실패시 시리얼 모니터 출력
    Serial.println("Failed to read from DHT");
    } else {
  //값 읽기 성공하면 온 습도 시리얼 모니터에 출력
  // Serial monitor 출력 시작
    Serial.print("SOIL HUMI: ");
    Serial.print(soil);
    Serial.println(" \t");
    Serial.print("TEMP: "); 
    Serial.print(temp);
    Serial.print(" ℃");
    Serial.println(" ");
    Serial.print("HUMI: "); 
    Serial.print(humi);
    Serial.println(" %\t");
    delay(3000); // 딜레이 나중에 올려야할듯
  // Serial monitor 출력 끝
  // 블루투스 모니터 작동 시작
  // 블루투스시리얼을 통해 문자열 전송
    lcd.display(); //lcd 표시
    lcd.begin(0x3F,20,4);
    lcd.print("SOIL HUMI: ");
    lcd.print(soil);
    lcd.setCursor(0,1);
    lcd.print("TEMP: ");
    lcd.print(temp);
    lcd.print(" *C");
    lcd.setCursor(0,2);
    lcd.print("HUMI: ");
    lcd.print(humi);
    lcd.print(" %");
    lcd.setCursor(0,3);

    BTSerial.println("                                "); // 이거 없으면 화면 깨짐
    BTSerial.print("SOIL HUMI: ");
    BTSerial.println(soil);
    BTSerial.print("TEMP: "); 
    BTSerial.print(temp);
    BTSerial.println(" ℃");
    BTSerial.print("HUMI: "); 
    BTSerial.print(humi);
    BTSerial.println(" %");
  
  delay(20); // 나중에 딜레이 올려야 함 10000정도

    if(soil >= 400){ // soil-moisture sensor의 값이 900 이상이면 모터 작동
      digitalWrite(Relay, HIGH);
      Serial.println("MOTOR RUN!"); // 시리얼 모니터에 표시
    } else { // 모터 정지
      digitalWrite(Relay, LOW);
      Serial.println("MOTOR STOP!"); // 시리얼 모니터에 표시
    } 
  //블루투스 모니터 작동 끝
  //블루 투스 통신 시작
  if (BTSerial.available()) { // 블루투스에서 data를 받을 수 있으면
    char data = BTSerial.read();  // 앱에서 보내는 데이터
    Serial.write(BTSerial.read()); // 시리얼 모니터에 표시
    Serial.write(data); //앱에서 보내는 신호 확인
    // 앱의 수동 급수 버튼 사용 on & off
    if(data == '1'){ // on 버튼을 누르면
      digitalWrite(Relay, HIGH);
      Serial.println("MOTOR MANUAL RUN!"); // 시리얼 모니터에 표시
     
    }else if(data == '0'){ // off 버튼을 누르면
      digitalWrite(Relay, LOW);
      Serial.println("MOTOR MANUAL STOP!");
    }
  }
} 
if (Serial.available()) {
    BTSerial.write(Serial.read());
  }  
}

