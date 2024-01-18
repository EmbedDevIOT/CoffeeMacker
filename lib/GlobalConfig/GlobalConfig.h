#ifndef GlobalConfig_H
#define GlobalConfig_H

// Include Global library Arduino Framework
#include <Arduino.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "SoftwareSerial.h"
#include "ESP8266WiFi.h"
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>

// #include <AsyncElegantOTA.h>
// #include <ESPAsyncTCP.h>
// #include <ESPAsyncWebServer.h>
#include <ESP8266WebServer.h>
#include <ElegantOTA.h>

#include "PubSubClient.h"
#include "ArduinoJSON.h"
#include "DFRobotDFPlayerMini.h"

#define DISABLE 0
#define ENABLE 1

// Speed Serial Port
#define BaudSpeed 9600

// Wifi Network preset 
// #define WORK_NET

// Player Connection
#define DFP_TX D1
#define DFP_RX D2

// Device PINs control
#define PWR_PIN D0
#define ST_PIN A0
#define ESP_PIN D5
#define LUN_PIN D6
#define CAP_PIN D7

// DS18b20 PIN connect 
// D0 - work 
#define ONE_WIRE_BUS D3


struct CNF
{
  String firmware = "1.8"; // Firmware version
#ifdef WORK_NET
  const char *ssid = "Keenetic-L3-2.4-prg";         // WiFi Login workNet
  const char *password = "QFCxfXMA3";               // WiFi Pass
#else
  const char *ssid = "EMBNET2G";         // WiFi Login
  const char *password = "Ae19co90$!eT"; // WiFi Pass
#endif
  bool power = false;
  float tC = 0.0;
  uint16_t cup_cnt = 0;
};

struct MQTT
{
  // MQTT broker credentials (set to NULL if not required)
  const char *username = "u_4YVJEF";
  const char *password = "v1HPYZgn";
  const char *server = "m5.wqtt.ru";
  const int port = 10073;
};

struct TIM
{
  const uint8_t TIM10 =  10, TIM100 = 100;
  uint16_t counter = 0;
  uint8_t state = 0;
  uint8_t tim10 = 0;
  uint8_t tim100 = 0;
  uint32_t tim1000 = 0;
};

struct TOP
{
  const String device_topic = "/devCM";
  String prog = "/devCM/progs";
  String cnt = "cnt";
  String pwrState = "/PwrST";
  String temp = "/Temp";
};

// Coffee enumeration
enum Coffee
{
  Espresso = 1,
  DoubleEspresso,
  Cappuccino
};

// Sound enumeration
enum SOUND
{
  PowerON = 1, // 0001
  CleanSyst,
  EspressoSet,
  LungoSet,
  CappSet, // dse
  EspressoReady,
  LungoReady,
  CappReady,
  PowerOFF
};

// extern ConfigD config;
extern CNF DevConfig;
extern TOP Topics;
extern TIM Timers;
extern MQTT mqtt;

void SystemInit();
void GPIOInit();

#endif