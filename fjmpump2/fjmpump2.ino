#include <Arduino.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif
// #include "config.h"
#include <ESPDateTime.h>

#include "WiFiInfo.h"

// don't forget to change this to real ssid/password
// or set these in config.h and include it
#ifndef WIFI_SSID
#define WIFI_SSID "Change to your WiFi SSID"
#define WIFI_PASS "Change to your WiFi Password"
#endif

unsigned long lastMs = 0;

unsigned long ms = millis();

void setupIO() {
  pinMode(2,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(15,OUTPUT);
  pinMode(17,OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  digitalWrite(2,0);
  digitalWrite(4,0);
  digitalWrite(15,0);
  digitalWrite(17,0);
  digitalWrite(13,0);
  digitalWrite(12,0);
}

void setupWiFi() {  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println(millis());
  Serial.print("WiFi Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(12,1);
    delay(1000);
    digitalWrite(12,0);
  }
  digitalWrite(12,1);
  Serial.println();
}

void setupDateTime() {
  // setup this after wifi connected
  // you can use custom timeZone,server and timeout
  // DateTime.setTimeZone(-4);
  DateTime.setServer("0.jp.pool.ntp.org");
  //   DateTime.begin(15 * 1000);
  DateTime.setTimeZone(9);
  DateTime.begin();
  while (!DateTime.isTimeValid()) {
    Serial.println("Failed to get time from server.");
    digitalWrite(12,0);
    delay(200);
    digitalWrite(12,1);
    delay(200);
  }
}

/**
 * More examples and docs see :
 * https://github.com/mcxiaoke/ESPDateTime
 *
 */

void setup() {
  Serial.begin(115200);  
  delay(1000);

  setupIO();
  setupWiFi();
  setupDateTime();

}

void delay1m(){
  delay(60000);
}

void loop() {
    digitalWrite(17, 1);
    digitalWrite(13, 1);
    delay1m();
    digitalWrite(17, 0);
    digitalWrite(13, 0);
    for(int i=0; i<59; i++){
      delay1m();
    }
}
    }
}
