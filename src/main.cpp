/*
# MQTT Service: https://dash.wqtt.ru/
# Log: gmail acc
# Author: EmbeddevIOT (Aleksey Baranov)
# Date: (create to 13.01.24)
# Discription: Control WiFi + MQTT + Yandex
# ########### Hardware ###########
# MCU: ESP8266
# Hardware: UART DFPlayer / DS18b20
# Elegant OTA Update
# mqtt_name: u_4YVJEF
# mqtt_pass: v1HPYZgn
# mqtt_server: m5.wqtt.ru
# mqtt_port: 10073
*/

// General Header
#include "GlobalConfig.h"

boolean pin_state = false;
boolean TASK = false;
boolean one_min = false;
boolean five_sec = false;
uint16_t tim_counter = 0;

// Global structs
CNF DevConfig;
TIM Timers;
TOP Topics;
MQTT mqtt;

// Themperature sensor Dallas DS18b20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Software UART control to DFPlayer
SoftwareSerial DFPSerial(/*rx =*/DFP_RX, /*tx =*/DFP_TX);
DFRobotDFPlayerMini myDFPlayer;
/**** Secure WiFi Connectivity Initialisation *****/
WiFiClientSecure espClient;
/**** MQTT Client Initialisation Using WiFi Connection *****/
PubSubClient client(espClient);
/***** Webserver object **********/
// AsyncWebServer server(80); // Create object on port 80
ESP8266WebServer server(80); // Create object on port 80
// AsyncWebSocket ws("/ws");

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

// Function Prototyps
void WiFiConnection(); // Wifi initializing
void StartOTA();
void Task10ms();
void Task100ms();
void Task1000ms();
void TaskMIN();
void reconnect();
void callback(char *topic, byte *payload, unsigned int length);
void publishMessage(const char *topic, String payload, boolean retained);
void updateLedState(void);
void ButtonClick(uint8_t pin);

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

  DFPSerial.begin(BaudSpeed);
  myDFPlayer.begin(DFPSerial);

  // Start the DS18B20 sensor
  sensors.begin();

#ifdef ESP8266
  espClient.setInsecure();
#else
  espClient.setCACert(root_ca); // enable this line and the the "certificate" code for secure connection
#endif
  // Settings for MQTT server
  client.setServer(mqtt.server, mqtt.port);
  client.setCallback(callback);

  // Handle the client request
  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  //           { request->send(200, "text/plain", "Hi! I am ESP8266."); });
  server.on("/", []()
            { server.send(200, "text/plain", "Hi! This is ElegantOTA Demo."); });
  // Start ElegantOTA
  // AsyncElegantOTA.begin(&server);
  ElegantOTA.begin(&server); // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();
  if (!client.connected())
    reconnect(); // check if client is connected

  Task10ms();
  Task100ms();
  Task1000ms();
  TaskMIN();
}

void Task10ms()
{
  if (millis() - Timers.tim10 >= 10)
  {
    Timers.tim100 += 10;
    TASK = !TASK;
    if (TASK)
    {
      client.loop();
    }
    else
    {
      ElegantOTA.loop();
      // AsyncElegantOTA.loop();
    }
  }
}

void Task100ms()
{
  if (millis() - Timers.tim100 >= 100)
  {
    Timers.tim100 += 100;

    // uint8_t power = digitalRead(ST_PIN);

    if (analogRead(ST_PIN) >= 500)
    {
      DevConfig.power = true; // Check power state
      // Serial.println("State_ON");
    }
    else
      DevConfig.power = false; // Check power state
  }
}

void Task1000ms()
{
  if (millis() - Timers.tim1000 >= 1000)
  {
    Timers.tim1000 += 1000;

    sensors.requestTemperatures();
    DevConfig.tC = sensors.getTempCByIndex(0);

    Serial.println(analogRead(ST_PIN));

    if (tim_counter < 60)
    {
      tim_counter++;
    }
    else
    {
      tim_counter = 0;
      one_min = true;
    }

    publishMessage((Topics.pwrState).c_str(), String(DevConfig.power).c_str(), true);

    // publishMessage("EmbedevIO", mqtt_message, true);
    // publishMessage(Topics.pwrState, String(DevConfig.power).c_str, true);
    // publishMessage(Topics.cnt.c_str(), String(counter).c_str(), true);
    // publishMessage(Topics.ledState.c_str(), String(LED.state).c_str(), true);
  }
}


// Send temperature (every one min)
void TaskMIN()
{
  if (one_min)
  {
    one_min = false;
    publishMessage((Topics.temp).c_str(), String(DevConfig.tC).c_str(), true);
    Serial.print(DevConfig.tC);
    Serial.println("ºC");
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
    if (client.connect(clientId.c_str(), mqtt.username, mqtt.password))
    {
      Serial.println("connected");

      client.subscribe((Topics.device_topic + "/#").c_str());
      client.subscribe((Topics.pwrState).c_str()); // subscribe the topics here
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
  // Serial.println(topic);
  // Serial.println(data_pay);

  if (String(topic) == Topics.device_topic)
  {
    if (data_pay == "ON" || data_pay == "1")
    {
      DevConfig.power = true;
      myDFPlayer.play(PowerON);
      delay(2000);
      ButtonClick(PWR_PIN);
      // Выбрать задержку
      delay(2000);
      myDFPlayer.play(CleanSyst);
      Serial.println("Power ON");
    }

    if (data_pay == "OFF" || data_pay == "0")
    {
      DevConfig.power = false;
      ButtonClick(PWR_PIN);

      myDFPlayer.play(PowerOFF);
      Serial.println("Power OFF");
    }
    publishMessage((Topics.pwrState).c_str(), String(DevConfig.power).c_str(), true);
  }

  if (String(topic) == Topics.prog)
  {
    String temp = data_pay;
    uint8_t tmp_prog = temp.toInt();

    switch (tmp_prog)
    {
    case Espresso:
      myDFPlayer.play(EspressoSet);
      delay(2000);
      ButtonClick(ESP_PIN);
      delay(10000);
      myDFPlayer.play(EspressoReady);
      break;
    case DoubleEspresso:
      myDFPlayer.play(LungoSet);
      delay(3000);
      ButtonClick(LUN_PIN);
      delay(10000);
      myDFPlayer.play(LungoReady);
      break;
    case Cappuccino:
      myDFPlayer.play(CappSet);
      delay(3000);
      ButtonClick(CAP_PIN);
      delay(10000);
      myDFPlayer.play(CappReady);
      break;

    default:
      break;
    }
    // publishMessage("/CM_Progs", String(temp).c_str(), true);
  }

  // updateLedState();
}

/**** Method for Publishing MQTT Messages **********/
void publishMessage(const char *topic, String payload, boolean retained)
{
  if (client.publish(topic, payload.c_str(), true))
    Serial.println("Message publised [" + String(topic) + "]: " + payload);
}

void updateLedState(void)
{
}

void StartOTA()
{
}

void ButtonClick(uint8_t pin)
{
  Serial.println("Click");
  digitalWrite(pin, HIGH);
  delay(500);
  digitalWrite(pin, LOW);
}
