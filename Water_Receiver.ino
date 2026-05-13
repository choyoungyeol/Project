#include <SPI.h>
#include <LoRa.h>

#define relayPin 7

float values[7]; // NodeID + 6개 데이터

bool relayState = false;  // 현재 릴레이 상태
int relayState_value = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);  // 릴레이 초기 상태 OFF

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String incoming = "";

    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }

    incoming.trim();

    // 데이터 파싱
    float values[7];
    int index = 0;
    int lastIndex = 0;

    for (int i = 0; i < incoming.length(); i++) {
      if (incoming.charAt(i) == ',' || i == incoming.length() - 1) {
        String part;
        if (i == incoming.length() - 1)
          part = incoming.substring(lastIndex);
        else
          part = incoming.substring(lastIndex, i);

        values[index++] = part.toFloat();
        lastIndex = i + 1;
        if (index >= 7) break;
      }
    }

    if (index >= 7) {
      int nodeID = (int)values[0];
      float Moisture = values[1];
      float root_Temp = values[2];
      float Temp = values[3];
      float Humi = values[4];
      float DewT = values[5];
      float EC = values[6];


      // 히스테리시스 조건 (릴레이 제어)
      if (!relayState && Temp > 31.0) {
        digitalWrite(relayPin, HIGH);
        relayState = true;
        relayState_value = 1;
      } else if (relayState && Temp < 25.0) {
        digitalWrite(relayPin, LOW);
        relayState = false;
        relayState_value = 0;
      }

      // ▶️ 한 줄로 출력
      Serial.print(incoming);
      Serial.print(",");
      Serial.println(relayState_value);  // 마지막만 println
    } else {
      Serial.println("파싱 실패");
    }
  }
}
