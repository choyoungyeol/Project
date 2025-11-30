#include <Wire.h>
#include "RTClib.h"

#define WaterPin1 A0
#define WaterPin2 A1
#define WaterPin3 A2
#define WaterPin4 A3

#define RelayPin1 6
#define RelayPin2 7
#define RelayPin3 8
#define RelayPin4 9
#define RelayPin5 10
#define RelayPin6 11
#define RelayPin7 12
#define RelayPin8 13

RTC_DS3231 rtc;

int Light1 = 0;
int Light2 = 0;
int Light3 = 0;
int Light4 = 0;
int Auto = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);
  pinMode(RelayPin5, OUTPUT);
  pinMode(RelayPin6, OUTPUT);
  pinMode(RelayPin7, OUTPUT);
  pinMode(RelayPin8, OUTPUT);
}

void loop() {
  String data = "";
  DateTime now = rtc.now();

  int Water1 = analogRead(WaterPin1);
  int WaterC1 = map(Water1, 480, 270, 0, 100);
  int Water2 = analogRead(WaterPin2);
  int WaterC2 = map(Water2, 500, 280, 0, 100);
  int Water3 = analogRead(WaterPin3);
  int WaterC3 = map(Water3, 500, 270, 0, 100);
  int Water4 = analogRead(WaterPin4);
  int WaterC4 = map(Water4, 500, 280, 0, 100);

  WaterC1 = constrain(WaterC1, 0, 100);
  WaterC2 = constrain(WaterC2, 0, 100);
  WaterC3 = constrain(WaterC3, 0, 100);
  WaterC4 = constrain(WaterC4, 0, 100);

  // Serial.print(now.year(), DEC);
  // Serial.print("/");
  // Serial.print(now.month(), DEC);
  // Serial.print("/");
  // Serial.print(now.day(), DEC);
  // Serial.print("    ");
  // Serial.print(now.hour(), DEC);
  // Serial.print(":");
  // Serial.print(now.minute(), DEC);
  // Serial.print(":");
  // Serial.print(now.second(), DEC);
  // Serial.println();

  // Serial.print("1 : ");
  // Serial.print(Water1);
  // Serial.print(", ");
  // Serial.println(WaterC1);
  // Serial.print("2 : ");
  // Serial.print(Water2);
  // Serial.print(", ");
  // Serial.println(WaterC2);
  // Serial.print("3 : ");
  // Serial.print(Water3);
  // Serial.print(", ");
  // Serial.println(WaterC3);
  // Serial.print("4 : ");
  // Serial.print(Water4);
  // Serial.print(", ");
  // Serial.println(WaterC4);
  // delay(1000);
  if (Auto == 1) {
    if ((now.hour() >= 6) && (now.hour() <= 18)) {
      digitalWrite(RelayPin1, HIGH);
      digitalWrite(RelayPin2, HIGH);
      digitalWrite(RelayPin3, HIGH);
      digitalWrite(RelayPin4, HIGH);
      Light1 = 1;
      Light2 = 1;
      Light3 = 1;
      Light4 = 1;

      // if (WaterC1 < 10) {
      //   digitalWrite(RelayPin5, HIGH);
      // } else {
      //   digitalWrite(RelayPin5, LOW);
      // }
      // if (WaterC2 < 10) {
      //   digitalWrite(RelayPin6, HIGH);
      // } else {
      //   digitalWrite(RelayPin6, LOW);
      // }
      // if (WaterC3 < 10) {
      //   digitalWrite(RelayPin7, HIGH);
      // } else {
      //   digitalWrite(RelayPin7, LOW);
      // }
      // if (WaterC4 < 10) {
      //   digitalWrite(RelayPin8, HIGH);
      // } else {
      //   digitalWrite(RelayPin8, LOW);
      // }
      Auto = 1;
    } else {
      digitalWrite(RelayPin1, LOW);
      digitalWrite(RelayPin2, LOW);
      digitalWrite(RelayPin3, LOW);
      digitalWrite(RelayPin4, LOW);
      digitalWrite(RelayPin5, LOW);
      digitalWrite(RelayPin6, LOW);
      digitalWrite(RelayPin7, LOW);
      digitalWrite(RelayPin8, LOW);
      Light1 = 0;
      Light2 = 0;
      Light3 = 0;
      Light4 = 0;
      Auto = 0;
    }
  }

  if (Serial.available()) {
    delay(3);
    char c = Serial.read();
    if (c == 'x') {
      Auto = 1;
    }
    if (c == 'y') {
      Auto = 0;
    }
    if (c == 'a') {
      digitalWrite(RelayPin1, HIGH);
      Light1 = 1;
    }
    if (c == 'b') {
      digitalWrite(RelayPin1, LOW);
      Light1 = 0;
    }
    if (c == 'c') {
      digitalWrite(RelayPin2, HIGH);
      Light2 = 1;
    }
    if (c == 'd') {
      digitalWrite(RelayPin2, LOW);
      Light2 = 0;
    }
    if (c == 'e') {
      digitalWrite(RelayPin3, HIGH);
      Light3 = 1;
    }
    if (c == 'f') {
      digitalWrite(RelayPin3, LOW);
      Light3 = 0;
    }
    if (c == 'g') {
      digitalWrite(RelayPin4, HIGH);
      Light4 = 1;
    }
    if (c == 'h') {
      digitalWrite(RelayPin4, LOW);
      Light4 = 0;
    }
  }
  Auto = Auto;
  data = String(Auto) + ", " + String(Light1) + ", " + String(WaterC1) + ", " + String(Light2) + ", " + String(WaterC2) + ", " + String(Light3) + ", " + String(WaterC3) + ", " + String(Light4) + ", " + String(WaterC4)+ ", ";
  Serial.println(data);
  delay(5000);
}
