#ifndef GlobalConfig_H
#define GlobalConfig_H

#include <Arduino.h>

#include "SoftwareSerial.h"
#include "ESP8266WiFi.h"
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include "PubSubClient.h"
#include "ArduinoJSON.h"
#include "DFRobotDFPlayerMini.h"

#define DISABLE 0
#define ENABLE 1

// Player Connection
#define DFP_TX D1
#define DFP_RX D2

// Device PINs control
#define PWR_PIN D0
#define ST_PIN A0
#define ESP_PIN D5
#define LUN_PIN D6
#define CAP_PIN D7

// Speed Serial Port
#define BaudSpeed 9600

struct ConfigD {
  String Ssid = "Keenetic-L3-2.4-prg";     // Имя сети для подключения 
  String Password = "QFCxfXMA3";           // Пароль сети для подключения 
  byte LedStartHour = 18;                  // Время запуска подсветки (часы)
  byte LedStartMinute = 30;                // Время запуска подсветки (минуты)
  byte LedFinishHour = 18;      
  byte LedFinishMinute = 30;
  byte LedOnOFF = 1;              // Флаг старта работы подсветки 
  byte LedPWM = 60;               // Яркость подсветки
  byte IP1 = 192;
  byte IP2 = 168;
  byte IP3 = 1;
  byte IP4 = 31;
  byte GW1 = 192;
  byte GW2 = 168;
  byte GW3 = 1;
  byte GW4 = 1;
  byte MK1 = 255;
  byte MK2 = 255;
  byte MK3 = 255;
  byte MK4 = 0;
  byte WiFiMode = 1; // Режим работы WiFi
};


// struct TIM
// {
//     uint8_t tim100 = 0;
//     uint32_t tim1000 = 0;
// };

// struct TOP
// {
//   String cnt = "cnt";
//   String ledState = "ledst";
// };


extern ConfigD config;
// extern TOP Topics;
// extern TIM Timers;


#endif 