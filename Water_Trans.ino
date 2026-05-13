#include <Sensirion.h>
#include <SDISerial.h>
#include <LoRa.h>
#include <Wire.h>
#include "RTClib.h"

#define WaterPin 6

int nodeID = 5;

const uint8_t dataPin  =  4;
const uint8_t clockPin =  5;

float air_temperature;
float air_humidity;
float air_dewpoint;
float fTemp;

SDISerial connection(WaterPin);
Sensirion tempSensor = Sensirion(dataPin, clockPin);

char* resp;
String outString = "";

RTC_DS3231 rtc;

void setup()
{
  rtc.begin();
  connection.begin();
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender");
  if (!LoRa.begin(915E6)) { // or 915E6, the MHz speed of yout module
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  delay(1000);
}

void loop()
{
  DateTime now = rtc.now();

  bool resetDone = false;

  if (now.hour() == 4 && now.minute() == 0 && !resetDone) {
    resetDone = true;
    void(* resetFunc) (void) = 0;
    resetFunc();
  }

  if (now.hour() == 4 && now.minute() > 1) {
    resetDone = false;
  }
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
  delay(60000);

  outString = String(nodeID) + "," + String(VWC) + "," + String(Temp) + "," + String(air_temperature) + "," + String(air_humidity) + "," + String(air_dewpoint) + "," + String(EC);

  LoRa.beginPacket();
  LoRa.println(outString);
  //Serial.println(outString);
  LoRa.endPacket();
  delay(50);
}
