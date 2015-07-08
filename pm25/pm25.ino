// PM 2.5 Project
// https://github.com/miaoski/pm25
//
#include "DHT.h"
#include <SoftwareSerial.h>

#define DHTTYPE DHT22
#define DHTPIN 3        // DHT pin to D3
#define SHARPPIN A0     // Pin 2 VO of dust sensor to Arduino A0
#define MQ9PIN   A1     // MQ9 SIG to Arduino A1
#define LEDPOWER 2      // Pin 4 LED VCC of dest sensor to Arduino D2
#define NODEMCU_RESET 9	// To NodeMCU reset pin to D9
#define APMODE_PIN 8	// Delete stored password in NodeMCU, GND pin D8
#define SSID_RESET 5	// Pull low to reset SSID / PassKey

#define samplingTime 280
#define deltaTime    40
#define sleepTime    9680

#undef  ESP8266_DBGMSG

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial esp8266(10, 11); // RX, TX

void setup() {
  pinMode(LEDPOWER, OUTPUT);
  Serial.begin(9600);
  delay(100);
  esp8266.begin(9600);
  delay(100);
  Serial.println("g0v PM2.5 Project");

  pinMode(SSID_RESET, INPUT_PULLUP);
  if(digitalRead(SSID_RESET) == 0) {
    Serial.println("Reset SSID/PassKey");
    esp8266.println("tmr.stop(0)");
    delay(100);
    esp8266.println("file.remove('config.lua')");
    delay(100);
    esp8266.println("node.restart()");
    Serial.println("Please set SSID/PassKey before resetting the device.");
    while(1)
      delay(1000);
  }
  
  // Reset NodeMCU
  pinMode(NODEMCU_RESET, OUTPUT);
  digitalWrite(NODEMCU_RESET, LOW);
  delay(100);
  digitalWrite(NODEMCU_RESET, HIGH);

  delay(500);
  dht.begin();
}
  
void loop() {
  int i;
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float MQ9Value = readMQ9();
  float dustVal = 0;

  for(i = 0; i < 250; i++) {
    dustVal = dustVal + read_dn7c3ca006();
  }
  dustVal = dustVal / i;

  // float calcVoltage = 0;
  // float dustDensity = 0;
  // calcVoltage = dustVal * (5.0 / 1024.0);
  // Check datasheet
  // dustDensity = 0.1724 * (calcVoltage - 0.6) * 1000.0;

  Serial.print("hum = "); 
  Serial.print(h);
  Serial.print("%   ");
  Serial.print("Temp: "); 
  Serial.print(t);
  Serial.print("C   ");
  Serial.print("MQ9_val: ");
  Serial.print(MQ9Value);
  Serial.print("   ");
  Serial.print("dn7c3ca006_val = ");
  Serial.println(dustVal);
  
  esp8266.print("hum = ");
  esp8266.println(h);
  delay(150);
  esp8266.print("tmp = ");
  esp8266.println(t);
  delay(150);
  esp8266.print("mq9 = ");
  esp8266.println(MQ9Value);
  delay(150);
  esp8266.print("pm25 = ");
  esp8266.println(dustVal);

#ifdef ESP8266_DBGMSG
  while(esp8266.available()) {
    Serial.write(esp8266.read());
  }
#endif

  delay(500);
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
  //sensor_volt = sensorValue/1024*5.0;
  //return sensor_volt;
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
