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
  tempSensor.measure(&air_temperature, &air_humidity, &air_dewpoint);

  resp = connection.sdi_query("?M!", 1000);
  delay(1000);
  resp = connection.sdi_query("?D0!", 1000);
  delay(2000);
  //Serial.println(resp);
  delay(1000);
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

  outString = String(ch) + "," + String(VWC) + "," + String(Temp) + "," + String(air_temperature) + "," + String(air_humidity) + "," + String(air_dewpoint) + "," + String(EC);

  Serial.println(outString);
  delay(2000);
  if (Serial.available()) {
    delay(3);
    char c = Serial.read();
    if (c == 'a') {
      digitalWrite(Relay1, HIGH);
    }
    if (c == 'b') {
      digitalWrite(Relay1, LOW);
    }
    if (c == 'c') {
      digitalWrite(Relay2, HIGH);
    }
    if (c == 'd') {
      digitalWrite(Relay2, LOW);
    }
    if (c == 'e') {
      digitalWrite(Relay3, HIGH);
    }
    if (c == 'f') {
      digitalWrite(Relay3, LOW);
    }
    if (c == 'g') {
      digitalWrite(Relay4, HIGH);
    }
    if (c == 'h') {
      digitalWrite(Relay4, LOW);
    }
  }
}
