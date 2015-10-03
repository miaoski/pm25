// PM 2.5 Project
// https://github.com/miaoski/pm25
//
#include "DHT.h"
#include <SoftwareSerial.h>

// define and undef connected devices
// DHT is mandatory.
#undef  DN7C3CA006
#define G3
#undef  ESP8266_DBGMSG
#undef  G3_DEBUG

// define pins
// DHT
#define DHTTYPE DHT22
#define DHTPIN        13    // DHT pin

// DN7C3CA006
#define SHARPPIN      A0   // Pin 2 VO of dust sensor
#define LEDPOWER      3    // Pin 4 LED VCC of dest sensor
#define samplingTime  460  // factory: 280, 40, 9680
#define deltaTime     40
#define sleepTime     9500

// ESP8266
#define NODEMCU_RESET 11   // Power-on reset NodeMCU
#define SSID_RESET    12   // Pull low to reset SSID / PassKey
SoftwareSerial esp8266(2, 3); // RX, TX

// G3
SoftwareSerial g3(8, 9); // RX, TX


// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
#ifdef DN7C3CA006
float vs;                       // Reference voltage of DN7C3CA006
#endif

void setup() {
  Serial.begin(9600);
  esp8266.begin(9600);
#ifdef DN7C3CA006
  pinMode(LEDPOWER, OUTPUT);
#endif
#ifdef G3
  g3.begin(9600);
#endif
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
  #ifdef DN7C3CA006
  vs = 110.0;
  #endif
  
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
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float dustVal = 0;
  int g3 = 0;
  
  Serial.print(F("hum = ")); 
  Serial.print(h);
  Serial.print(F("%   "));
  Serial.print(F("temp: ")); 
  Serial.print(t);
  Serial.print(F("C   "));
#ifdef DN7C3CA006
  dustVal = readDN7C3CA006(h);
  Serial.print(F("dust_val = "));
  Serial.print(dustVal);
  Serial.print(F(" mg/m3   "));
#endif
#ifdef G3
  g3 = readG3();
  Serial.print(F("G3 = "));
  Serial.print(g3);
  Serial.print(F(" mg/m3"));
#endif
  Serial.println("");
  
  esp8266.print(F("hum = "));
  esp8266.println(h);
  delay(150);
  esp8266.print(F("tmp = "));
  esp8266.println(t);
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

#ifdef DN7C3CA006
float readDN7C3CA006(float humidity) {
  unsigned short i;
  float v0;
  float dn7c_h;
  float dustVal;

  v0 = 0;
  for(i = 0; i < 50; i++) {
    v0 = v0 + read_dn7c3ca006();
  }
  v0 = v0 / i;
  Serial.print(F("v0 = "));
  Serial.print(v0);
  Serial.print(F("   "));

  if(humidity < 50) {
    dn7c_h = 1;
  } else {
    dn7c_h = 1 - 0.01467 * (humidity - 50);
  }
  if(v0 > vs) {
    dustVal = 0.6 * dn7c_h * (v0 - vs) * 5.0 * 1000.0 / 1024.0;
  } else {
    dustVal = 0;
  }
  return dustVal;
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
#endif

#ifdef G3
int readG3() {
  unsigned short x;
  byte data;
  byte buf[24];
  boolean start = false;
  memset(buf, 0, 24);
  for(x = 0; x < 24;) {
    if(g3.available()) {
      data = g3.read();
      if(!start && data != 0x42) {
        continue;
      }
      start = true;
      buf[x++] = data;
    }
  }
#ifdef G3_DEBUG
    for(int i = 0; i < 24; i++) {
      Serial.print("i = ");
      Serial.print(i);
      Serial.print(" => ");
      Serial.println((unsigned int)buf[i]);
    }
#endif
  if(G3CheckSum(buf)) {
//    cf1 = ((unsigned int)buf[6] << 8) + (unsigned int)buf[7];
      x = ((unsigned int)buf[12] << 8) + (unsigned int)buf[13];
      return x;
  } else {
#ifdef G3_DEBUG
      Serial.println(F("G3: invalid checksum"));
#endif
    return -1;
  }
}

boolean G3CheckSum(byte *p) {
  unsigned short i;
  unsigned int calcsum = 0; // BM
  unsigned int exptsum;
  for(i = 0; i < 22; i++) {
    calcsum += (unsigned int)p[i];
  }
  exptsum = ((unsigned int)p[22] << 8) + (unsigned int)p[23];
  if(calcsum == exptsum) {
    return true;
  } else {
#ifdef G3_DEBUG
    Serial.print(F("G3: Expected = "));
    Serial.print(exptsum);
    Serial.print(F("  calculated = "));
    Serial.println(calcsum);
#endif
    return false;
  }
}
#endif
