#include <SDISerial.h>
#include <Wire.h>

#define WaterPin 7

SDISerial connection(WaterPin);
char* resp;
String outString = "";

void setup() {
  connection.begin();
  Serial.begin(9600);
  delay(1000);
}

void loop() {
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

  outString = String(ch) + "," + String(VWC) + "," + String(EC) + "," + String(Temp);

  Serial.println(outString);
  delay(2000);
}
