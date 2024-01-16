/*
# MQTT Service: https://dash.wqtt.ru/
# Author: EmbeddevIOT (Aleksey Baranov)
# Date: (create to 13.01.24)
# Discription:
# ########### Hardware ###########
# MCU: ESP8266
# Control: WiFi + MQTT + Yandex
# Hardware: UART DFPlayer
# OTA Update
# mqtt_name: u_4YVJEF
# mqtt_pass: v1HPYZgn
# mqtt_server: m5.wqtt.ru
# mqtt_port: 10073
*/

#include "GlobalConfig.h"

// Global structs
CNF DevConfig;
TIM Timers;
TOP Topics;
MQTT mqtt;

// Software UART control to DFPlayer
SoftwareSerial DFPSerial(/*rx =*/DFP_RX, /*tx =*/DFP_TX);
DFRobotDFPlayerMini myDFPlayer;
/**** Secure WiFi Connectivity Initialisation *****/
WiFiClientSecure espClient;
/**** MQTT Client Initialisation Using WiFi Connection *****/
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

uint8_t track = 0;

// Function Prototyps
void WiFiConnection();
void StartOTA();

void setup()
{
  Serial.begin(BaudSpeed);
  Serial.println("Boot");
  for (uint8_t i = 0; i < 10; i++)
  {
    Serial.print(".");
    delay(10);
  }
  Serial.printf("Firmware:");
  Serial.println(DevConfig.firmware);

  WiFiConnection();
  delay(10);

  DFPSerial.begin(9600);
  myDFPlayer.begin(DFPSerial);


  // #ifdef ESP8266
  //   espClient.setInsecure();
  // #else
  //   espClient.setCACert(root_ca); // enable this line and the the "certificate" code for secure connection
  // #endif

  // client.setServer(mqtt_server, mqtt_port);
  // client.setCallback(callback);
}

void loop()
{
  static unsigned long timer = millis();
    // Serial.println(F("next"));

  // if (millis() - timer > 7000)
  // {
  //   timer = millis();
  //   track ++;
  //   Serial.printf("track: %d",track);
  //   Serial.println();
  //   // myDFPlayer.reset();
  //   myDFPlayer.play(track); // Play next mp3 every 3 second.
  //   if(track == 9) track = 1;
  // }
  // ArduinoOTA.handle();
  // Task1000ms();

  // if ((LED.show == true) && (LED.state == true))
  // {
  //   static uint32_t tmr;

  //   if (millis() - tmr >= 5)
  //   {
  //     tmr = millis();
  //     static int8_t dir = 1;
  //     static int val = 0;
  //     val += dir;
  //     if (val == 255 || val == 0)
  //       dir = -dir; // разворачиваем
  //     led.setBrightness(val);
  //   }
  // }

  // if (!client.connected())
  //   reconnect(); // check if client is connected
  // client.loop();
}

void WiFiConnection()
{
  Serial.println("Connecting Wifi.");
  WiFi.mode(WIFI_STA);
  WiFi.begin(DevConfig.ssid, DevConfig.password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
}

void StartOTA()
{
}
