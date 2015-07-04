// PM 2.5 Project
// https://github.com/miaoski/pm25
//
#include "DHT.h"
#include <SoftwareSerial.h>

#define DHTTYPE DHT22
#define DHTPIN 3        // DHT pin
#define SHARPPIN A0     // Pin 2 VO of dust sensor to Arduino A0
#define MQ9PIN   A1     // MQ9 SIG to Arduino A1
#define LEDPOWER 2      // Pin 4 LED VCC of dest sensor to Arduino D2
#define NODEMCU_RESET 9	// To NodeMCU reset pin

#define samplingTime 280
#define deltaTime    40
#define sleepTime    9680

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial esp8266(10, 11); // RX, TX

void setup() {
  pinMode(LEDPOWER, OUTPUT);
  Serial.begin(9600);
  delay(250);
  Serial.println("g0v PM2.5 Project");
  
  // Reset NodeMCU
  pinMode(NODEMCU_RESET, OUTPUT);
  digitalWrite(NODEMCU_RESET, LOW);
  delay(100);
  digitalWrite(NODEMCU_RESET, HIGH);

  delay(500);
  esp8266.begin(9600);
  dht.begin();
}
  
void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float MQ9Value = readMQ9();
  float dustVal = read_dn7c3ca006();

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

  delay(2000);
}

float readMQ9() {
  float sensor_volt; 
  float sensorValue;
  sensorValue = 0;
  for(int x = 0 ; x < 100 ; x++) {
    sensorValue += analogRead(MQ9PIN);
  }
  sensorValue = sensorValue/100.0;
  return sensorValue;
  //sensor_volt = sensorValue/1024*5.0;
  //return sensor_volt;
}

float read_dn7c3ca006() {
  float dust;
  digitalWrite(LEDPOWER, LOW);  // power on the LED
  delayMicroseconds(samplingTime);
  dust = analogRead(SHARPPIN);  // read the dust value
  delayMicroseconds(deltaTime);
  digitalWrite(LEDPOWER, HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
  return dust;
}
