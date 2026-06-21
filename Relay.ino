#define Relay1 8
#define Relay2 9
#define Relay3 10
#define Relay4 11

void setup() {
  // put your setup code here, to run once:
  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(Relay1, HIGH);
  delay(1000);
  digitalWrite(Relay1, LOW);
  delay(2000);
  digitalWrite(Relay2, HIGH);
  delay(1000);
  digitalWrite(Relay2, LOW);
  delay(2000);
  digitalWrite(Relay3, HIGH);
  delay(1000);
  digitalWrite(Relay3, LOW);
  delay(2000);
  digitalWrite(Relay4, HIGH);
  delay(1000);
  digitalWrite(Relay4, LOW);
  delay(2000);
}
