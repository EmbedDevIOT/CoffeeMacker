/*
# MQTT Service: https://dash.wqtt.ru/
# Author: EmbeddevIOT (Aleksey Baranov)
# Date: (create to 13.01.24)
# Discription:
# ########### Hardware ###########
# MCU: ESP8266
# Control: WiFi + MQTT + Yandex
# Hardware: DFPlayer
# mqtt_name: u_4YVJEF
# mqtt_pass: v1HPYZgn
# mqtt_server: m5.wqtt.ru
# mqtt_port: 10073
*/

#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <WiFiClientSecure.h>
#include "PubSubClient.h"
#include "ArduinoJSON.h"

#define BaudSpeed 9600
static const char firmware[] = {"0.1"};

// WiFi Login and Password
const char *ssid = "EMBNET2G";
const char *password = "";
/**** Secure WiFi Connectivity Initialisation *****/
WiFiClientSecure espClient;

uint16_t counter = 0;
uint8_t state = 0;

struct Timers
{
  uint8_t tim100 = 0;
  uint32_t tim1000 = 0;
};
Timers TIM;

// MQTT broker credentials (set to NULL if not required)
const char *mqtt_username = "u_4YVJEF";
const char *mqtt_password = "v1HPYZgn";
// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char *mqtt_server = "m5.wqtt.ru";
const int mqtt_port = 10073;
/**** MQTT Client Initialisation Using WiFi Connection *****/
PubSubClient client(espClient);

const String leds_topic = "/led";

struct topics
{
  String cnt = "cnt";
  String ledState = "ledst";
};
topics TOPIC;

// char mqtt_message[128];

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

void setup()
{
  Serial.begin(BaudSpeed);
  Serial.println(F("MQTT_Tracker_Started"));
  Serial.printf("Firmware: %s", firmware);

  setup_wifi();

#ifdef ESP8266
  espClient.setInsecure();
#else
  espClient.setCACert(root_ca); // enable this line and the the "certificate" code for secure connection
#endif

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop()
{
  Task1000ms();

  if ((LED.show == true) && (LED.state == true))
  {
    static uint32_t tmr;

    if (millis() - tmr >= 5)
    {
      tmr = millis();
      static int8_t dir = 1;
      static int val = 0;
      val += dir;
      if (val == 255 || val == 0)
        dir = -dir; // разворачиваем
      led.setBrightness(val);
    }
  }

  if (!client.connected())
    reconnect(); // check if client is connected
  client.loop();
}
