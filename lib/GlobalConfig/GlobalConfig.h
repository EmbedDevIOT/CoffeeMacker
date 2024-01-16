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

struct CNF
{
  String firmware = "1.0";               // Firmware version
  const char *ssid = "EMBNET2G";         // WiFi Login
  const char *password = "Ae19co90$!eT"; // WiFi Pass
  bool power = false;
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
  uint16_t counter = 0;
  uint8_t state = 0;
  uint8_t tim100 = 0;
  uint32_t tim1000 = 0;
};

struct TOP
{
  const String device_topic = "/devCM";
  String prog = "/devCM/progs";
  String cnt = "cnt";
  String pwrState = "/PwrST";
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