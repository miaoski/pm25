// PM 2.5 Project
// https://github.com/miaoski/pm25
//
#include "DHT.h"
#include "ESP8266.h"
#include <SoftwareSerial.h>

#define DHTTYPE DHT22
#define DHTPIN 3        // DHT pin
#define SHARPPIN A0     // Pin 2 VO of dust sensor to Arduino A0
#define MQ9PIN   A1     // MQ9 SIG to Arduino A1
#define LEDPOWER 2      // Pin 4 LED VCC of dest sensor to Arduino D2

#define samplingTime 280
#define deltaTime    40
#define sleepTime    9680

#define SSID "ljm"
#define APKEY "ftrftrftr"
#define HOST_NAME "220.135.254.63"  // miaoski.idv.tw
#define HOST_PORT (2080)

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial dbgSerial(10, 11); // RX, TX
ESP8266 wifi(Serial);

void setup() {
  pinMode(LEDPOWER, OUTPUT);
  Serial.begin(115200);
  Serial.setTimeout(5000);
  delay(1000);
  dbgSerial.begin(9600);
  dbgSerial.println("g0v PM2.5 Project");

  dbgSerial.print("FW Version:");
  dbgSerial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStationSoftAP()) {
    dbgSerial.print("to station + softap ok\r\n");
  } else {
    dbgSerial.print("to station + softap err\r\n");
  }

  for (int i = 0; i < 5; i++) { // retry 5 times
    if (wifi.joinAP(SSID, APKEY)) {
      dbgSerial.print("Join AP success\r\n");
      dbgSerial.print("IP:");
      dbgSerial.println(wifi.getLocalIP().c_str());
    } else {
      dbgSerial.print("Join AP failure\r\n");
    }
  }

  if (wifi.disableMUX()) {
    dbgSerial.print("single ok\r\n");
  } else {
    dbgSerial.print("single err\r\n");
  }

  dbgSerial.print("setup end\r\n");
  dht.begin();
}
  
void loop() {
  uint8_t buffer[128] = {0};
  char url[256];
  char h_str[7];  // 100.00
  char t_str[7];  // 100.00
  char m_str[7];  // 999.99
  char d_str[8];  // 9999.99

  if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
    dbgSerial.print("create tcp ok\r\n");
  } else {
    dbgSerial.print("create tcp err\r\n");
    delay(500);
    return;
  }

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float MQ9Value = readMQ9();
  float dustVal = read_dn7c3ca006();

  // float calcVoltage = 0;
  // float dustDensity = 0;
  // calcVoltage = dustVal * (5.0 / 1024.0);
  // Check datasheet
  // dustDensity = 0.1724 * (calcVoltage - 0.6) * 1000.0;

  dbgSerial.print("Hum: "); 
  dbgSerial.print(h);
  dbgSerial.print("%   ");
  dbgSerial.print("Temp: "); 
  dbgSerial.print(t);
  dbgSerial.print("C   ");
  dbgSerial.print("MQ9_val: ");
  dbgSerial.print(MQ9Value);
  dbgSerial.print("   ");
  dbgSerial.print("dn7c3ca006_val = ");
  dbgSerial.println(dustVal);

  dtostrf(h, 4, 2, h_str);
  dtostrf(t, 4, 2, t_str);
  dtostrf(MQ9Value, 4, 2, m_str);
  dtostrf(dustVal, 5, 2, d_str);  
  snprintf(url, sizeof(url), 
    "GET /hum/%s/temp/%s/mq9/%s/dn7/%s HTTP/1.0\r\n\r\n",
    h_str, t_str, m_str, d_str);
  wifi.send((const uint8_t*)url, strlen(url));
  dbgSerial.println(url);

  /*
  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
  if (len > 0) {
    dbgSerial.print("Received:[");
    for (uint32_t i = 0; i < len; i++) {
      dbgSerial.print((char)buffer[i]);
    }
    dbgSerial.print("]\r\n");
  }
  */

  if (wifi.releaseTCP()) {
    dbgSerial.print("release tcp ok\r\n");
  } else {
    dbgSerial.print("release tcp err\r\n");
  }
  
  //Serial.print("  dustVal: ");
  //Serial.print(dustVal);

  //if (dustVal>36.455) {
  //  Serial.print("  PM2.5: ");
    // 公式複製自 http://tw.taobao.com/item/44139051810.htm
  //  Serial.print((float(dustVal/1024)-0.0356)*120000*0.035);
  //  Serial.print("  Dust Density: ");
  //  Serial.print(dustDensity);
  //  Serial.println(" ug/m3 ");
  //} else {
  //  Serial.print("   Invalid voltage: ");
  //  Serial.println(dustVal);
  //}
  delay(5000);
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
  digitalWrite(LEDPOWER,LOW); // power on the LED
  delayMicroseconds(samplingTime);
  dust = analogRead(SHARPPIN); // read the dust value
  delayMicroseconds(deltaTime);
  digitalWrite(LEDPOWER,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
  return dust;
}
