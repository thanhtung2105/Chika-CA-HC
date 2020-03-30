#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>

#define ledR 16
#define ledB 5
#define btn_config 4

uint32_t timer = 0;
uint16_t longPressTime = 6000;

boolean buttonActive = false;

const char *mqtt_server = "chika.gq";
const int mqtt_port = 2502;
const char *mqtt_user = "chika";
const char *mqtt_pass = "2502";

const char *CA_SWR = "2b92934f-7a41-4ce1-944d-d33ed6d97e13/stateDevice";
const char *CA_SWR2 = "4a0bfbfe-efff-4bae-927c-c8136df70333/stateDevice";
const char *CA_SWR3 = "ebb2464e-ba53-4f22-aa61-c76f24d3343d/stateDevice";

Ticker ticker;
WiFiClient esp;
PubSubClient client(esp);

void tick();
void tick2();
void exitSmartConfig();
boolean startSmartConfig();
void longPress();
void callback(char *topic, byte *payload, unsigned int length);
void reconnect();

void setup()
{
  Serial.begin(115200);

  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);

  pinMode(ledR, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(btn_config, INPUT);

  ticker.attach(1, tick2);
  uint16_t i = 0;
  while (!WiFi.isConnected())
  {
    i++;
    delay(100);
    if (i >= 100)
      break;
  }
  if (!WiFi.isConnected())
  {
    startSmartConfig();
  }
  else
  {
    ticker.detach();
    digitalWrite(ledR, LOW);
  }

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop()
{
  /*
  // if (Serial.available())
  // {
  //   String payload = Serial.readString();
  //   Serial.println(payload);
  // }

  String Json_CA_SWR, Json_CA_SWR2, Json_CA_SWR3;
  StaticJsonDocument<200> CA_SWR;
  StaticJsonDocument<200> CA_SWR2;
  StaticJsonDocument<200> CA_SWR3;

  CA_SWR["type"] = "2b92934f-7a41-4ce1-944d-d33ed6d97e13";
  CA_SWR["button_1"] = true;
  CA_SWR["button_2"] = "";
  CA_SWR["button_3"] = "";
  serializeJson(CA_SWR, Json_CA_SWR);
  Serial.print(Json_CA_SWR);
  Serial.flush();
  delay(2000);

  CA_SWR2["type"] = "4a0bfbfe-efff-4bae-927c-c8136df70333";
  CA_SWR2["button_1"] = false;
  CA_SWR2["button_2"] = true;
  CA_SWR2["button_3"] = "";
  serializeJson(CA_SWR2, Json_CA_SWR2);
  Serial.print(Json_CA_SWR2);
  Serial.flush();
  delay(2000);

  CA_SWR3["type"] = "ebb2464e-ba53-4f22-aa61-c76f24d3343d";
  CA_SWR3["button_1"] = false;
  CA_SWR3["button_2"] = true;
  CA_SWR3["button_3"] = false;
  serializeJson(CA_SWR3, Json_CA_SWR3);
  Serial.print(Json_CA_SWR3);
  Serial.flush();
  delay(2000);
  */

  //longPress();
  if (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(ledB, HIGH);
    digitalWrite(ledR, LOW);
    if (client.connected())
    {
      client.loop();
    }
    else
    {
      reconnect();
    }
  }
  else
  {
    WiFi.reconnect();
    digitalWrite(ledB, LOW);
    boolean state = digitalRead(ledR);
    digitalWrite(ledR, !state);
  }
}

void reconnect()
{
  while (!client.connected())
  {
    String clientId = "CA-HC_combination - ";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass))
    {
      Serial.println("Connected");
      Serial.flush();
      client.subscribe(CA_SWR);
      client.subscribe(CA_SWR2);
      client.subscribe(CA_SWR3);
    }
    else
    {
      delay(1000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
    for (unsigned int i = 0; i < length; i++)
    {
      Serial.print((char)payload[i]);
    }
    Serial.flush();
}

void tick()
{
  boolean state = digitalRead(ledR);
  digitalWrite(ledR, !state);
}

void tick2()
{
  boolean state = digitalRead(ledR);
  digitalWrite(ledR, !state);
  digitalWrite(ledB, !state);
}

void exitSmartConfig()
{
  WiFi.stopSmartConfig();
  ticker.detach();
  digitalWrite(ledR, LOW);
  digitalWrite(ledB, HIGH);
}

boolean startSmartConfig()
{
  uint16_t t = 0;
  Serial.println("On SmartConfig ");
  WiFi.beginSmartConfig();
  delay(500);
  ticker.attach(0.1, tick);
  while (WiFi.status() != WL_CONNECTED)
  {
    t++;
    Serial.print(".");
    delay(500);
    if (t > 100)
    {
      ticker.attach(0.5, tick);
      delay(3000);
      exitSmartConfig();
      return false;
    }
  }
  Serial.println("WiFi connected ");
  Serial.print("IP :");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.SSID());
  exitSmartConfig();
  return true;
}

void longPress()
{
  if (digitalRead(btn_config) == HIGH)
  {
    if (buttonActive == false)
    {
      buttonActive = true;
      timer = millis();
      Serial.println(timer);
    }

    if (millis() - timer > longPressTime)
    {
      digitalWrite(ledB, LOW);
      startSmartConfig();
    }
  }
  else
  {
    buttonActive = false;
  }
}