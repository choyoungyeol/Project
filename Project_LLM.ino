#include <SDISerial.h>
#include <Sensirion.h>

#define WaterPin 7
#define Relay1 8
#define Relay2 9
#define Relay3 10
#define Relay4 11

const uint8_t dataPin = 2;
const uint8_t clockPin = 3;

float air_temperature;
float air_humidity;
float air_dewpoint;
float fTemp;

Sensirion tempSensor = Sensirion(dataPin, clockPin);
SDISerial connection(WaterPin);
char* resp;
String outString = "";

// 아두이노 코드 수정 예시
unsigned long lastChangeTime = 0;
const unsigned long minInterval = 60000;  // 최소 1분은 상태 유지 (60초)

void setup() {
  connection.begin();
  Serial.begin(9600);
  delay(1000);
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    unsigned long currentTime = millis();

    // 마지막 제어 후 일정 시간이 지났는지 확인
    if (currentTime - lastChangeTime > minInterval) {
      if (c == 'a') {
        digitalWrite(Relay1, HIGH);
        lastChangeTime = currentTime;
      }
      if (c == 'b') {
        digitalWrite(Relay1, LOW);
        lastChangeTime = currentTime;
      }
      if (c == 'c') {
        digitalWrite(Relay2, HIGH);
        lastChangeTime = currentTime;
      }
      if (c == 'd') {
        digitalWrite(Relay2, LOW);
        lastChangeTime = currentTime;
      }
      if (c == 'e') {
        digitalWrite(Relay3, HIGH);
        lastChangeTime = currentTime;
      }
      if (c == 'f') {
        digitalWrite(Relay3, LOW);
        lastChangeTime = currentTime;
      }
      if (c == 'g') {
        digitalWrite(Relay4, HIGH);
        lastChangeTime = currentTime;
      }
      if (c == 'h') {
        digitalWrite(Relay4, LOW);
        lastChangeTime = currentTime;
      }
    }
  }
  tempSensor.measure(&air_temperature, &air_humidity, &air_dewpoint);
  resp = connection.sdi_query("?M!", 1000);
  delay(1000);
  resp = connection.sdi_query("?D0!", 1000);
  delay(1000);

  // 2. 데이터 유효성 검사 (매우 중요!)
  // resp가 비어있거나 데이터가 없으면 출력하지 않도록 방어
  if (resp == NULL || String(resp).length() < 5) {
    return;  // 유효한 값이 없으면 이번 루프는 건너뜀
  }

  String resp1 = resp;
  int first = resp1.indexOf("+");
  int second = resp1.indexOf("+", first + 1);
  int third = resp1.indexOf("+", second + 1);
  int length = resp1.length();

  String ch = resp1.substring(0, first);
  String VWC = resp1.substring(first + 1, second);
  String EC = resp1.substring(second + 1, third);
  String Temp = resp1.substring(third + 1, length);
  delay(2000);

  // 4. 데이터 출력
  String outString = "{\"ch\":\"" + String(ch) + "\",";
  outString += "\"vwc\":" + String(VWC) + ",";
  outString += "\"temp\":" + String(Temp) + ",";
  outString += "\"air_temperature\":" + String(air_temperature) + ",";
  outString += "\"air_humidity\":" + String(air_humidity) + ",";
  outString += "\"air_dewpoint\":" + String(air_dewpoint) + ",";
  outString += "\"EC\":" + String(EC) + "}";

  Serial.println(outString);
  delay(2000);
}
