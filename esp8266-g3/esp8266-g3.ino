// PM 2.5 Project
// https://github.com/miaoski/pm25
// Direct connect G3 to ESP8266

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT22

#define SSID        "SSID"
#define PASSWORD    "PASSWORD"
#define HOST_NAME   "220.135.254.63"
#define HOST_PORT   443
#define UUID        "00000000-0000-0000-0000-000000000000"

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  dht.begin();
  Serial.begin(9600);     // Remember to AT+UART=9600,8,1,0,0
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("WiFi connected");
  delay(1000);
}

void loop() {
  WiFiClientSecure client;
  char str[256];
  int g3 = 0;
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  char t_s[6];
  char h_s[6];

  dtostrf(t, 0, 0, t_s);
  dtostrf(h, 0, 0, h_s);
  g3 = readG3();
  Serial.print("g3 = ");
  Serial.print(g3);
  Serial.println();
  snprintf(str, sizeof(str),
    "GET /id/%s/hum/%s/tmp/%s/mq9/0/dn7/%d HTTP/1.0\r\n\r\n",
    UUID, h_s, t_s, g3);
  if(client.connect(HOST_NAME, HOST_PORT)) {
    client.print(str);
    Serial.print(str);
    delay(10);
  }
  delay(10 * 1000);
}

int readG3() {
  unsigned short x;
  unsigned short cf1;
  byte data;
  byte buf[24];
  boolean start = false;
  memset(buf, 0, 24);
  for(x = 0; x < 24;) {
    if(Serial.available()) {
      data = Serial.read();
      if(!start && data != 0x42) {
        continue;
      }
      start = true;
      buf[x++] = data;
    }
  }
  if(G3CheckSum(buf)) {
    cf1 = ((unsigned int)buf[6] << 8) + (unsigned int)buf[7];
    x = ((unsigned int)buf[12] << 8) + (unsigned int)buf[13];
    return x;
  } else {
    return -1;
  }
}

void blink_dead(int high_time) {
  while(1) {
    digitalWrite(BUILTIN_LED, HIGH);
    delay(high_time);
    digitalWrite(BUILTIN_LED, LOW);
    delay(1000);
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
    return false;
  }
}
