// Use library from https://github.com/itead/ITEADLIB_Arduino_WeeESP8266
// GPLv2

#include "ESP8266.h"
#include <SoftwareSerial.h>

#define SSID "ljm"
#define APKEY "ftrftrftr"
#define HOST_NAME "220.135.254.63"  // miaoski.idv.tw
#define HOST_PORT (2080)

SoftwareSerial dbgSerial(10, 11); // RX, TX
ESP8266 wifi(Serial);

void setup() {
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
}

void loop() {
  uint8_t buffer[128] = {0};

  if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
    dbgSerial.print("create tcp ok\r\n");
  } else {
    dbgSerial.print("create tcp err\r\n");
    delay(500);
    return;
  }

  char *url = "GET /from_arduino HTTP/1.0\r\n\r\n";
  wifi.send((const uint8_t*)url, strlen(url));

  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
  if (len > 0) {
    dbgSerial.print("Received:[");
    for (uint32_t i = 0; i < len; i++) {
      dbgSerial.print((char)buffer[i]);
    }
    dbgSerial.print("]\r\n");
  }

  if (wifi.releaseTCP()) {
    dbgSerial.print("release tcp ok\r\n");
  } else {
    dbgSerial.print("release tcp err\r\n");
  }
  delay(5000);
}
