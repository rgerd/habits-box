#pragma once

/*
  Arduino Nano 33 IoT WiFi Demo
  Based upon Arduino example by Arturo Guadalupi
*/

// Include required libraries
#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <RTCZero.h>
#include "secrets/WifiConfig.h"

// WiFi Credentials (stored in WifiConfig.h)
char ssid[] = WIFI_SSID;  // Wifi SSID
char pass[] = WIFI_PASS;  // Wifi password
int keyIndex = 0;         // Network key Index number (needed only for WEP)

constexpr int GMT = -7; // PST

RTCZero rtc;

inline void _SetUpRTC() {
  rtc.begin();
  unsigned long epoch;
  do
  {
    epoch = WiFi.getTime();
    analogWrite(LED_BUTTON_LIGHT_PIN, ((millis() / 500) % 2) * 255);
  } while (epoch == 0);
  rtc.setEpoch(epoch + GMT * 3600);
  Serial.print("Received epoch: ");
  Serial.println(epoch);
}

inline String _GetDateString()
{
  return String(rtc.getDay()) + "-" + String(rtc.getMonth()) + "-" + String(rtc.getYear());
}

inline void _PrintWiFiStatus() {
  // Print the network SSID
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Print the IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // Print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
