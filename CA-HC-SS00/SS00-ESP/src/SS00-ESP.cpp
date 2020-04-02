#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <Ticker.h>
#include <ArduinoJson.h>

/* This product will send data with sampling cycle is 60s */

#define DHTPIN 0
#define DHTTYPE DHT22
#define ledR 16
#define ledB 5
#define btn_config 4

DHT SS00(DHTPIN, DHTTYPE);
uint32_t timer_sendTempHumi = 0;
uint32_t timer = 0;
uint16_t longPressTime = 6000;

boolean buttonActive = false;

const char *mqtt_server = "chika.gq";
const int mqtt_port = 2502;
const char *mqtt_user = "chika";
const char *mqtt_pass = "2502";

//Topic: HC_id/temphumi
const char *CA_SS00 = "f7a3bde5-5a85-470f-9577-cdbf3be121d4";

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
}

void loop()
{
  delay(100);
  if (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(ledB, HIGH);
    digitalWrite(ledR, LOW);
    if (client.connected())
    {
      client.loop();

      timer_sendTempHumi++;
      if (timer_sendTempHumi > 60)     // with timer = 10 equal to 1s
      {
        timer_sendTempHumi = 0;
        float h = SS00.readHumidity();
        float t = SS00.readTemperature();

        while (isnan(h) || isnan(t))
        {
          h = SS00.readHumidity();
          t = SS00.readTemperature();
        }

        Serial.print("Humidity: ");
        Serial.print(h);
        Serial.print(" %\n");
        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.println(" oC\n");

        String sendTempHumi;
        char payload_sendTempHumi[500];
        StaticJsonDocument<500> JsonCA_SS00;

        JsonCA_SS00["type"] = "f7a3bde5-5a85-470f-9577-cdbf3be121d4";
        JsonCA_SS00["temperature"] = t;
        JsonCA_SS00["humidity"] = h;
        serializeJson(JsonCA_SS00, sendTempHumi);

        sendTempHumi.toCharArray(payload_sendTempHumi, sendTempHumi.length() + 1);
        client.publish(CA_SS00, payload_sendTempHumi, true);
        Serial.println("MQTT sent !");
      }
    }
    else
    {
      reconnect();
    }
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
    }
    else
    {
      Serial.println("Reconnect in 1 second !");
      delay(1000);
    }
  }
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
  WiFi.beginSmartConfig();
  delay(500);
  ticker.attach(0.1, tick);
  while (WiFi.status() != WL_CONNECTED)
  {
    t++;
    delay(500);
    if (t > 100)
    {
      ticker.attach(0.5, tick);
      delay(3000);
      exitSmartConfig();
      return false;
    }
  }
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