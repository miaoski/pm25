#include <LiquidCrystal.h>

LiquidCrystal lcd(9, 8, 7, 6, 5, 4);  // RS, E, D4, D5, D6, D7

void setup() {
  lcd.begin(16, 2);
  lcd.print("hello, world!");
}

void loop() {
  lcd.setCursor(0, 1);
  lcd.print(millis()/1000);
  delay(1000);
}

