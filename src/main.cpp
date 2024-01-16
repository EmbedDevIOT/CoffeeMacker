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
void Task100ms();
void Task1000ms();
void reconnect();
void callback(char *topic, byte *payload, unsigned int length);
void publishMessage(const char *topic, String payload, boolean retained);
void updateLedState(void);

void setup()
{
  GPIOInit();
  Serial.begin(BaudSpeed);
  Serial.println("Boot");

  for (uint8_t i = 0; i < 10; i++)
  {
    Serial.print(".");
    delay(10);
  }

  Serial.println();
  Serial.printf("Firmware:");
  Serial.println(DevConfig.firmware);

  WiFiConnection();
  delay(10);

  DFPSerial.begin(9600);
  myDFPlayer.begin(DFPSerial);

#ifdef ESP8266
  espClient.setInsecure();
#else
  espClient.setCACert(root_ca); // enable this line and the the "certificate" code for secure connection
#endif

  client.setServer(mqtt.server, mqtt.port);
  client.setCallback(callback);
}

void loop()
{
  Task1000ms();

  // static unsigned long timer = millis();
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
  digitalWrite(PWR_PIN, HIGH);
  digitalWrite(ESP_PIN, HIGH);
  digitalWrite(LUN_PIN, HIGH);
  digitalWrite(CAP_PIN, HIGH);
  myDFPlayer.play(PowerON);
  delay(5000);

  digitalWrite(PWR_PIN, LOW);
  digitalWrite(ESP_PIN, LOW);
  digitalWrite(LUN_PIN, LOW);
  digitalWrite(CAP_PIN, LOW);
  myDFPlayer.play(PowerOFF);
  delay(5000);

  if (!client.connected())
    reconnect(); // check if client is connected
  client.loop();

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

void Task100ms()
{
  if (millis() - Timers.tim100 >= 100)
  {
    Timers.tim100 += 100;
  }
}

void Task1000ms()
{
  if (millis() - Timers.tim1000 >= 1000)
  {
    Timers.tim1000 += 1000;

    // publishMessage("EmbedevIO", mqtt_message, true);

    publishMessage(Topics.cnt.c_str(), String(counter).c_str(), true);
    publishMessage(Topics.ledState.c_str(), String(LED.state).c_str(), true);

    // publishMessage("led_state", String(state).c_str(), true);
  }
}

void WiFiConnection()
{
  delay(10);
  Serial.print("\nConnecting to ");
  Serial.println(DevConfig.ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(DevConfig.ssid, DevConfig.password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("\nWiFi connected\nIP address: ");
  Serial.println(WiFi.localIP());
}

/************* Connect to MQTT Broker ***********/
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-"; // Create a random client ID
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("connected");

      client.subscribe((leds_topic + "/#").c_str());

      // client.subscribe("led_state"); // subscribe the topics here
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds"); // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/***** Call back Method for Receiving MQTT messages and Switching LED ****/
void callback(char *topic, byte *payload, unsigned int length)
{
  String data_pay;
  for (int i = 0; i < length; i++)
  {
    data_pay += String((char)payload[i]);
  }
  Serial.println(topic);
  Serial.println(data_pay);

  if (String(topic) == leds_topic)
  {
    if (data_pay == "ON" || data_pay == "1")
    {
      LED.state = true;
      Serial.println("LED_ON");
    }
    if (data_pay == "OFF" || data_pay == "0")
    {
      LED.state = false;
      Serial.println("LED_OFF");
    }
  }

  if (String(topic) == (leds_topic + "/brig"))
  {
    uint8_t tmp = 0;
    LED.state = true;
    LED.bright = data_pay.toInt();
    publishMessage("st_brig", String(LED.bright).c_str(), true);
    tmp = map(LED.bright, 0, 100, 0, 255);
    LED.bright = tmp;
  }

  if (String(topic) == (leds_topic + "/color"))
  {
    LED.state = true;
    String temp_col = data_pay;
    LED.colHEX = temp_col.toInt();
    publishMessage("st_col", String(LED.colHEX).c_str(), true);
  }

  if (String(topic) == (leds_topic + "/show"))
  {
    String temp_col = data_pay;
    LED.show = temp_col.toInt();
    publishMessage("st_show", String(LED.show).c_str(), true);
  }

  updateLedState();
}

/**** Method for Publishing MQTT Messages **********/
void publishMessage(const char *topic, String payload, boolean retained)
{
  if (client.publish(topic, payload.c_str(), true))
    Serial.println("Message publised [" + String(topic) + "]: " + payload);
}

void updateLedState(void)
{
  if (LED.state == true)
  {
    led.setBrightness(LED.bright);
    led.setHEX(LED.colHEX);
  }
  else
    // led.disable();
    led.setBrightness(0);
}

void StartOTA()
{
}
