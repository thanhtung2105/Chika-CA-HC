/* This source code for CA-HC and it has included the functions for controlling list of following products:
        - CA-SWR: {"id":"2b92934f-7a41-4ce1-944d-d33ed6d97e13", "type":"1 button", "RF_channel": 1002502019001}
        - CA-SWR2: {"id":"4a0bfbfe-efff-4bae-927c-c8136df70333", "type":"2 buttons", "RF_channel": 1002502019002}
        - CA-SWR3: {"id":"ebb2464e-ba53-4f22-aa61-c76f24d3343d", "type":"3 buttons", "RF_channel": 1002502019003}
        - CA-SS00: {"id":"f7a3bde5-5a85-470f-9577-cdbf3be121d4", "type":"CA-SS00", "RF_channel": "none"}
        - CA-SS02: {"id":"9d860c55-7899-465b-9fb3-195ae0c0959a", "type":"CA-SS02", "RF_channel": 1002502019004}
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>

/* This product (CA-HC) will send data with sampling cycle is 60s */

#define DHTPIN 0
#define DHTTYPE DHT22
#define ledR 16
#define ledB 5
#define btn_config 4

DHT SS00(DHTPIN, DHTTYPE);
uint32_t timer_sendTempHumi = 0;

//Time for SmartConfig:
uint32_t timer = 0;
uint16_t longPressTime = 6000;

boolean buttonActive = false;

const char *mqtt_server = "chika.gq";
const int mqtt_port = 2502;
const char *mqtt_user = "chika";
const char *mqtt_pass = "2502";

//List topic of communication with products:
const char *CA_SS00 = "f7a3bde5-5a85-470f-9577-cdbf3be121d4";
const char *CA_SS02 = "9d860c55-7899-465b-9fb3-195ae0c0959a";

const char *CA_SWR = "2b92934f-7a41-4ce1-944d-d33ed6d97e13";
const char *CA_SWR2 = "4a0bfbfe-efff-4bae-927c-c8136df70333";
const char *CA_SWR3 = "ebb2464e-ba53-4f22-aa61-c76f24d3343d";

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
  SS00.begin();

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
  //longPress();
  delay(100);
  if (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(ledB, HIGH);
    digitalWrite(ledR, LOW);
    if (client.connected())
    {
      client.loop();

      //First - Check information from CA-SS00:
      timer_sendTempHumi++;
       if (timer_sendTempHumi > 30)     // with timer = 100 equal to 1s
       {
        timer_sendTempHumi = 0;
        float h = SS00.readHumidity();
        float t = SS00.readTemperature();

        // while (isnan(h) || isnan(t))
        // {
        //   h = SS00.readHumidity();
        //   t = SS00.readTemperature();
        //   delay(5);
        // }

        Serial.print("Humidity: ");
        Serial.print(h);
        Serial.print(" %\n");
        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.println(" oC\n");

        String sendTempHumi;
        char payload_sendTempHumi[500];
        StaticJsonDocument<500> JsonCA_SS00;

        JsonCA_SS00["type"] = CA_SS00;
        JsonCA_SS00["temperature"] = t;
        JsonCA_SS00["humidity"] = h;
        serializeJson(JsonCA_SS00, sendTempHumi);

        sendTempHumi.toCharArray(payload_sendTempHumi, sendTempHumi.length() + 1);
        client.publish(CA_SS00, payload_sendTempHumi, true);
      }
      
      //Send state of device when having anything changes from product:
      if (Serial.available())
      {
        String payload_MEGA = Serial.readString();
        // Serial.println(payload_MEGA);

        StaticJsonDocument<200> JsonDoc;
        deserializeJson(JsonDoc, payload_MEGA);
        char payload_toChar[200];
        payload_MEGA.toCharArray(payload_toChar, payload_MEGA.length() + 1);

        String type = JsonDoc["type"];
        if (type == "CA-SWR")
        {
          client.publish(CA_SWR, payload_toChar);
        }
        else if (type == "CA-SWR2")
        {
          client.publish(CA_SWR2, payload_toChar);
        }
        else if (type == "CA-SWR3")
        {
          client.publish(CA_SWR3, payload_toChar);
        }
        else if (type == "CA-SS02")
        {
          client.publish(CA_SS02, payload_toChar);
        }
      }
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
      Serial.println("Connected !");
      Serial.flush();
      client.subscribe(CA_SS02);
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