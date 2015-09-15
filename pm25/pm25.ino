// PM 2.5 Project
// https://github.com/miaoski/pm25
//
#include "DHT.h"
#include <SoftwareSerial.h>

#define DHTTYPE DHT22
#define DHTPIN 2        // DHT pin
#define SHARPPIN A0     // Pin 2 VO of dust sensor
#define MQ9PIN   A1     // MQ9 SIG to Arduino
#define LEDPOWER 3      // Pin 4 LED VCC of dest sensor
#define NODEMCU_RESET 9	// To NodeMCU reset pin to
#define APMODE_PIN 8	// Delete stored password in NodeMCU, GND pin
#define SSID_RESET 12	// Pull low to reset SSID / PassKey
#define MQ9_PREHEAT  90 // 90+10 seconds

// 280, 40, 9680
#define samplingTime 460
#define deltaTime    40
#define sleepTime    9500

#undef  ESP8266_DBGMSG

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial esp8266(10, 11); // RX, TX
SoftwareSerial g3(8, 9); // RX, TX
float vs;                       // Reference voltage of DN7C3CA006

void setup() {
  pinMode(LEDPOWER, OUTPUT);
  Serial.begin(9600);
  esp8266.begin(9600);
  g3.begin(9600);
  delay(100);
  Serial.println(F("g0v PM2.5 Project"));

  pinMode(SSID_RESET, INPUT_PULLUP);
  if(digitalRead(SSID_RESET) == 0) {
    Serial.println(F("Reset SSID/PassKey"));
    esp8266.println(F("tmr.stop(0)"));
    delay(100);
    esp8266.println(F("tmr.stop(0)"));
    delay(100);
    esp8266.println(F("tmr.stop(0)"));
    delay(100);
    esp8266.println(F("tmr.stop(0)"));
    delay(100);
    esp8266.println(F("file.remove('config.lua')"));
    delay(200);
    esp8266.println(F("node.restart()"));
    Serial.println(F("Please set SSID/PassKey before resetting the device."));
    while(1) {
      while(esp8266.available()) {
        Serial.write(esp8266.read());
      }
    }
  }

  // TODO: Calibrate Vs of DN7C3CA006
  vs = 110.0;
  
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
  float v0;
  float dn7c_h;
  float dustVal;
  int g3;

  v0 = 0;
  for(i = 0; i < 50; i++) {
    v0 = v0 + read_dn7c3ca006();
  }
  v0 = v0 / i;
  Serial.print(F("v0 = "));
  Serial.print(v0);
  Serial.print(F("   "));

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

  g3 = readG3();
 
  Serial.print(F("hum = ")); 
  Serial.print(h);
  Serial.print(F("%   "));
  Serial.print(F("temp: ")); 
  Serial.print(t);
  Serial.print(F("C   "));
  Serial.print(F("dust_val = "));
  Serial.print(dustVal);
  Serial.print(F(" mg/m3   "));
  Serial.print(F("G3 = "));
  Serial.print(g3);
  Serial.println(F(" mg/m3"));
  
  esp8266.print(F("hum = "));
  esp8266.println(h);
  delay(150);
  esp8266.print(F("tmp = "));
  esp8266.println(t);
  delay(150);
  esp8266.print(F("mq9 = "));
  esp8266.println(g3);
  delay(150);
  esp8266.print(F("pm25 = "));
  esp8266.println(dustVal);

#ifdef ESP8266_DBGMSG
  while(esp8266.available()) {
    Serial.write(esp8266.read());
  }
#endif

  delay(2000);
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

int readG3() {
  int x;
  unsigned char buf[48];
  unsigned char *p;
  if(g3.available()) {
    g3.readBytes((char *)buf, 48);
    for(x = 0; x < 48 && buf[x] != 'B'; x++);
    for(; x < 48 && buf[x] != 'M'; x++);
    /*
    for(int i = x-1; i < x+23; i++) {
      Serial.print("i = ");
      Serial.print(i);
      Serial.print(" => ");
      Serial.println((unsigned int)buf[i]);
    }
    */
    p = buf + x - 1;
    if(G3CheckSum(p)) {
//    cf1 = ((unsigned int)p[6] << 8) + (unsigned int)p[7];
      x = ((unsigned int)p[12] << 8) + (unsigned int)p[13];
    } else {
      /*
      Serial.println(F("G3: invalid checksum"));
      */
      return -1;
    }
  } else {
    return -1;
  }
}

short G3CheckSum(unsigned char *p) {
  int i;
  unsigned int calcsum = 0; // BM
  unsigned int exptsum;
  for(i = 0; i < 22; i++) {
    calcsum += (unsigned int)p[i];
  }
  exptsum = ((unsigned int)p[22] << 8) + (unsigned int)p[23];
  if(calcsum == exptsum) {
    return 1;
  } else {
    /*
    Serial.print(F("G3: Expected = "));
    Serial.print(exptsum);
    Serial.print(F("  calculated = "));
    Serial.println(calcsum);
    */
    return 0;
  }
}

