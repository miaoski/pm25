#include <LiquidCrystal.h>

LiquidCrystal lcd(9, 8, 7, 6, 5, 4);  // RS, E, D4, D5, D6, D7
#define LEDPOWER 2      // Pin 4 LED VCC of dest sensor to Arduino D2
#define samplingTime 280
#define deltaTime    40
#define sleepTime    9680
#define SHARPPIN A0     // Pin 2 VO of dust sensor to Arduino A0

void setup() {
  Serial.begin(9600);
  pinMode(LEDPOWER, OUTPUT);
  lcd.begin(16, 2);
  lcd.print("hello, world!");
}

void loop() {
  int i;
  float v0;

  v0 = 0;
  for(i = 0; i < 1000; i++) {
    lcd.setCursor(0, 0);
    lcd.print(i);
    v0 = v0 + read_dn7c3ca006();
  }
  v0 = v0 / i;
  Serial.print("vs = ");
  Serial.println(v0);
  lcd.setCursor(0, 1);
  lcd.print("vs = ");
  lcd.print(v0);
}

float read_dn7c3ca006() {
  float dust = 0;
  digitalWrite(LEDPOWER, LOW);  // power on the LED
  delayMicroseconds(samplingTime);
  dust = analogRead(SHARPPIN);  // read the dust value
  delayMicroseconds(deltaTime);
  digitalWrite(LEDPOWER, HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
  return dust;
}
