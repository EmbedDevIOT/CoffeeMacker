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

ConfigD config;
// TIM Timers;
// TOP Topics;


// Firmware version
// Device.fw = "0.3";

static const char firmware[] = {"0.3"};

// Software UART control to DFPlayer
SoftwareSerial DFPSerial(/*rx =*/DFP_RX, /*tx =*/DFP_TX);
DFRobotDFPlayerMini myDFPlayer;

// WiFi Login and Password
const char *ssid = "EMBNET2G";
const char *password = "";

/**** Secure WiFi Connectivity Initialisation *****/
WiFiClientSecure espClient;

uint16_t counter = 0;
uint8_t state = 0;

// MQTT broker credentials (set to NULL if not required)
const char *mqtt_username = "u_4YVJEF";
const char *mqtt_password = "v1HPYZgn";
// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char *mqtt_server = "m5.wqtt.ru";
const int mqtt_port = 10073;
/**** MQTT Client Initialisation Using WiFi Connection *****/
PubSubClient client(espClient);

const String leds_topic = "/led";



// char mqtt_message[128];

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

void WiFiConnection();
void StartOTA();


void setup()
{
  Serial.begin(BaudSpeed);
  Serial.println(F("MQTT_Tracker_Started"));
  Serial.printf("Firmware: %s", firmware);

  WiFiConnection();
  delay(10);

  DFPSerial.begin(9600);
  myDFPlayer.begin(DFPSerial);

  // delay(10);
  // myDFPlayer.stop();
  // delay(10);
  // if (!myDFPlayer.begin(softSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
  //   Serial.println(F("Unable to begin:"));
  //   Serial.println(F("1.Please recheck the connection!"));
  //   Serial.println(F("2.Please insert the SD card!"));
  //   while(true){
  //     delay(0); // Code to compatible with ESP8266 watch dog.
  //   }
  // }
  // Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(5); // Set volume value. From 0 to 30
  delay(100);
  myDFPlayer.play(1); // Play the first mp3
  delay(10000);
  Serial.println(F("Track 1 stop"));
  // myDFPlayer.stop();
  myDFPlayer.volume(5); // Set volume value. From 0 to 30
  delay(100);
  Serial.println(F("Volume 5"));
  myDFPlayer.play(2);
  delay(100);
  Serial.println(F("Track 2 Play"));

  delay(100);

  // delay(100);
  // myDFPlayer.play(1);    // Play the first mp3

  // setup_wifi();

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
  WiFi.begin(ssid, password);

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
