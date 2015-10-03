// PM 2.5 Project
// https://github.com/miaoski/pm25
//
#include "DHT.h"
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

#define DHTTYPE DHT11
#define DHTPIN 2        // DHT pin to D2
#define SHARPPIN A0     // Pin 2 VO of dust sensor to Arduino A0
#define MQ9PIN   A1     // MQ9 SIG to Arduino A1
#define LEDPOWER 3      // Pin 4 LED VCC of dest sensor to Arduino D3
#define NODEMCU_RESET 9	// To NodeMCU reset pin to D9
#define APMODE_PIN 8	// Delete stored password in NodeMCU, GND pin D8
#define SSID_RESET 12	// Pull low to reset SSID / PassKey
#define MQ9_PREHEAT  90 // 90+10 seconds

#define samplingTime 460
#define deltaTime    40
#define sleepTime    9500

#undef  ESP8266_DBGMSG

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial esp8266(10, 11); // RX, TX
float vs;                       // Reference voltage of DN7C3CA006
LiquidCrystal lcd(4, 5, 6, 7, 8, 9);  // RS, E, D4, D5, D6, D7

void setup() {
  lcd.begin(16, 2);
  pinMode(LEDPOWER, OUTPUT);
  Serial.begin(9600);
  delay(100);
  esp8266.begin(9600);
  delay(100);
  Serial.println(F("g0v PM2.5 Project"));
  lcd.print("g0v PM2.5 Project");

  pinMode(SSID_RESET, INPUT_PULLUP);
  if(digitalRead(SSID_RESET) == 0) {
    Serial.println(F("Reset SSID/PassKey"));
    esp8266.println(F("tmr.stop(0)"));
    delay(100);
    esp8266.println(F("file.remove('config.lua')"));
    delay(100);
    esp8266.println(F("node.restart()"));
    Serial.println(F("Please set SSID/PassKey before resetting the device."));
    lcd.clear();
    lcd.print(F("Set SSID/Passkey and reset device"));
    while(1) {
      while(esp8266.available()) {
        Serial.write(esp8266.read());
      }
    }
  }

  // TODO: Calibrate Vs of DN7C3CA006
  vs = calibrate_DN7C();
  Serial.print(F("Calibrated vs = "));
  Serial.println(vs);
  
  // Reset NodeMCU
  /*
  pinMode(NODEMCU_RESET, OUTPUT);
  digitalWrite(NODEMCU_RESET, LOW);
  delay(100);
  digitalWrite(NODEMCU_RESET, HIGH);
  */

  delay(500);
  dht.begin();
}
  
void loop() {
  int i;
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float MQ9Value = readMQ9();
  float v0;
  float dn7c_h;
  float dustVal;

  // Value is meaningless before the sensor is hot
  if(millis() / 1000 < MQ9_PREHEAT) {
    MQ9Value = 0;
  }

  v0 = 0;
  for(i = 0; i < 25; i++) {
    v0 = v0 + read_dn7c3ca006();
  }
  v0 = v0 / i;
  Serial.print("v0 = ");
  Serial.println(v0);

  if(h < 50) {
    dn7c_h = 1;
  } else {
    dn7c_h = 1 - 0.01467 * (h - 50);
  }
  if(v0 > vs) {
    dustVal = 0.6 * dn7c_h * (v0 - vs) * 5.0 * 1000.0 / 1024.0;
  } else {
    dustVal = 0;
  }
  
  Serial.print(F("hum = ")); 
  Serial.print(h);
  Serial.print(F("%   "));
  Serial.print(F("temp: ")); 
  Serial.print(t);
  Serial.print(F("C   "));
  Serial.print(F("MQ9_val: "));
  Serial.print(MQ9Value);
  Serial.print(F("   "));
  Serial.print(F("dust_val = "));
  Serial.print(dustVal);
  Serial.println(F(" mg/m3"));

  lcd.clear();
  lcd.print(h); lcd.print("%RH ");
  lcd.print(t); lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print(MQ9Value); lcd.print("  ");
  lcd.print(dustVal); lcd.print("mg/m3");
  
  esp8266.print(F("hum = "));
  esp8266.println(h);
  delay(150);
  esp8266.print(F("tmp = "));
  esp8266.println(t);
  delay(150);
  esp8266.print(F("mq9 = "));
  esp8266.println(MQ9Value);
  delay(150);
  esp8266.print(F("pm25 = "));
  esp8266.println(dustVal);

#ifdef ESP8266_DBGMSG
  while(esp8266.available()) {
    Serial.write(esp8266.read());
  }
#endif

  delay(1000);
}

float readMQ9() {
  int x;
  float sensor_volt; 
  float sensorValue;
  sensorValue = 0;
  for(x = 0 ; x < 100 ; x++) {
    sensorValue += analogRead(MQ9PIN);
  }
  sensorValue = sensorValue/x;
  return sensorValue;
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

float calibrate_DN7C() {
  float v0 = 0;
  unsigned short i;
  for(i = 0; i < 100; i++) {
    v0 = v0 + read_dn7c3ca006();
    delay(100);
  }
  v0 = v0 / i;
  return v0;
}

