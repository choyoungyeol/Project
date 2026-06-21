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

// [전역 변수] loop() 밖에 선언해야 값이 유지됩니다.
unsigned long lastChangeRelay1 = 0;
unsigned long lastChangeRelay2 = 0;
unsigned long lastChangeRelay3 = 0;
unsigned long lastChangeRelay4 = 0;
const unsigned long minInterval = 2000;

void setup() {
  connection.begin();
  Serial.begin(115200);
  delay(1000);
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    unsigned long currentTime = millis();  // 여기서 현재 시간 확인

    // Relay1 (a, b)
    if (c == 'a' || c == 'b') {
      if (currentTime - lastChangeRelay1 >= minInterval) {
        digitalWrite(Relay1, (c == 'a') ? HIGH : LOW);
        lastChangeRelay1 = currentTime;
      }
    }
    // Relay2 (c, d)
    else if (c == 'c' || c == 'd') {
      if (currentTime - lastChangeRelay2 >= minInterval) {
        digitalWrite(Relay2, (c == 'c') ? HIGH : LOW);
        lastChangeRelay2 = currentTime;
      }
    }
    // Relay3 (e, f)
    else if (c == 'e' || c == 'f') {
      if (currentTime - lastChangeRelay3 >= minInterval) {
        digitalWrite(Relay3, (c == 'e') ? HIGH : LOW);
        lastChangeRelay3 = currentTime;
      }
    }
    // Relay4 (g, h)
    else if (c == 'g' || c == 'h') {
      if (currentTime - lastChangeRelay4 >= minInterval) {
        digitalWrite(Relay4, (c == 'g') ? HIGH : LOW);
        lastChangeRelay4 = currentTime;
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
