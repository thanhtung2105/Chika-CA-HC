#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>
#include <ArduinoJson.h>

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

boolean stateButton_received_value[3];
String Control_from_MQTT;

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
    Serial.println("WIFI CONNECTED");
    Serial.println(WiFi.SSID());
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop()
{
  //longPress();
  if (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(ledB, HIGH);
    digitalWrite(ledR, LOW);
    if (client.connected())
    {
      client.loop();
      // do something here
      if (Serial.available())
      {
        String payload = Serial.readString();
        Serial.println(payload);

        StaticJsonDocument<200> JsonDoc;
        deserializeJson(JsonDoc, payload);
        char payload_toChar[200];
        payload.toCharArray(payload_toChar, payload.length() + 1);

        String type = JsonDoc["type"];
        Serial.print("Type received: ");
        Serial.println(type);

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
      }
    }
    else
    {
      reconnect();
    }
  }
  else
  {
    Serial.println("WiFi Connected Fail");
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
    Serial.print("Attempting MQTT connection...");
    String clientId = "CA-HC_combination - ";
    clientId += String(random(0xffff), HEX);
    Serial.println(clientId);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass))
    {
      Serial.println("Connected");
      client.subscribe(CA_SWR);
      client.subscribe(CA_SWR2);
      client.subscribe(CA_SWR3);
    }
    else
    {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println("Try again in 1 second");
      delay(1000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  //Topic list test is the value of variables: CA_SWR | CA_SWR2_1 ; CA_SWR2_2 | CA_SWR3_1 ; CA_SWR3_2 ; CA_SWR3_3
  //Print message of button ID:

  // CA-SWR:
  if ((char)topic[38] == '3')
    switch ((char)payload[0])
    {
    case '1':
      stateButton_received_value[0] = true;
      JsonDoc_CfM["type"] = "CA-SWR1";
      JsonDoc_CfM["button_data"] = stateButton_received_value[0];
      serializeJson(JsonDoc_CfM, Control_from_MQTT);
      Serial.print(Control_from_MQTT);
      break;
    case '0':
      stateButton_received_value[0] = false;
      JsonDoc_CfM["type"] = "CA-SWR1";
      JsonDoc_CfM["button_data"] = stateButton_received_value[0];
      serializeJson(JsonDoc_CfM, Control_from_MQTT);
      Serial.print(Control_from_MQTT);
      break;
    }

  // CA-SWR2:
  if ((char)topic[38] == '4')
    switch ((char)payload[0])
    {
    case '1':
      // stateButton_MQTT_CA_SWR2[0] = 1;
      // Serial.println("CA_SWR2_1 - ON");
      // radio.stopListening();
      // radio.openWritingPipe(address_CA_SWR2);
      // radio.write(&stateButton_MQTT_CA_SWR2, sizeof(stateButton_MQTT_CA_SWR2));
      Serial.println("MQTT: CA-SWR2_1 callback - ON !");
      break;
    case '0':
      // stateButton_MQTT_CA_SWR2[0] = 0;
      // Serial.println("CA_SWR2_1 - OFF");
      // radio.stopListening();
      // radio.openWritingPipe(address_CA_SWR2);
      // radio.write(&stateButton_MQTT_CA_SWR2, sizeof(stateButton_MQTT_CA_SWR2));
      Serial.println("MQTT: CA-SWR2_1 callback - OFF !");
      break;
    }

  if ((char)topic[38] == 'a')
    switch ((char)payload[0])
    {
    case '1':
      // stateButton_MQTT_CA_SWR2[1] = 1;
      // Serial.println("CA_SWR2_2 - ON");
      // radio.stopListening();
      // radio.openWritingPipe(address_CA_SWR2);
      // radio.write(&stateButton_MQTT_CA_SWR2, sizeof(stateButton_MQTT_CA_SWR2));
      Serial.println("MQTT: CA-SWR2_2 callback - ON !");
      break;
    case '0':
      // stateButton_MQTT_CA_SWR2[1] = 0;
      // Serial.println("CA_SWR2_2 - OFF");
      // radio.stopListening();
      // radio.openWritingPipe(address_CA_SWR2);
      // radio.write(&stateButton_MQTT_CA_SWR2, sizeof(stateButton_MQTT_CA_SWR2));
      Serial.println("MQTT: CA-SWR2_2 callback - OFF !");
      break;
    }

  // CA-SWR3:
  if ((char)topic[38] == 'f')
    switch ((char)payload[0])
    {
    case '1':
      // stateButton_MQTT_CA_SWR3[0] = 1;
      // Serial.println("CA_SWR3_1 - ON");
      // radio.stopListening();
      // radio.openWritingPipe(address_CA_SWR3);
      // radio.write(&stateButton_MQTT_CA_SWR3, sizeof(stateButton_MQTT_CA_SWR3));
      Serial.println("MQTT: CA-SWR3_1 callback - ON !");
      break;
    case '0':
      // stateButton_MQTT_CA_SWR3[0] = 0;
      // Serial.println("CA_SWR3_1 - OFF");
      // radio.stopListening();
      // radio.openWritingPipe(address_CA_SWR3);
      // radio.write(&stateButton_MQTT_CA_SWR3, sizeof(stateButton_MQTT_CA_SWR3));
      Serial.println("MQTT: CA-SWR3_1 callback - OFF !");
      break;
    }

  if ((char)topic[38] == '5')
    switch ((char)payload[0])
    {
    case '1':
      // stateButton_MQTT_CA_SWR3[1] = 1;
      // Serial.println("CA_SWR3_2 - ON");
      // radio.stopListening();
      // radio.openWritingPipe(address_CA_SWR3);
      // radio.write(&stateButton_MQTT_CA_SWR3, sizeof(stateButton_MQTT_CA_SWR3));
      Serial.println("MQTT: CA-SWR3_2 callback - ON !");
      break;
    case '0':
      // stateButton_MQTT_CA_SWR3[1] = 0;
      // Serial.println("CA_SWR3_2 - OFF");
      // radio.stopListening();
      // radio.openWritingPipe(address_CA_SWR3);
      // radio.write(&stateButton_MQTT_CA_SWR3, sizeof(stateButton_MQTT_CA_SWR3));
      Serial.println("MQTT: CA-SWR3_2 callback - OFF !");
      break;
    }

  if ((char)topic[38] == 'b')
    switch ((char)payload[0])
    {
    case '1':
      // stateButton_MQTT_CA_SWR3[2] = 1;
      // Serial.println("CA_SWR3_3 - ON");
      // radio.stopListening();
      // radio.openWritingPipe(address_CA_SWR3);
      // radio.write(&stateButton_MQTT_CA_SWR3, sizeof(stateButton_MQTT_CA_SWR3));
      Serial.println("MQTT: CA-SWR3_3 callback - ON !");
      break;
    case '0':
      // stateButton_MQTT_CA_SWR3[2] = 0;
      // Serial.println("CA_SWR3_3 - OFF");
      // radio.stopListening();
      // radio.openWritingPipe(address_CA_SWR3);
      // radio.write(&stateButton_MQTT_CA_SWR3, sizeof(stateButton_MQTT_CA_SWR3));
      Serial.println("MQTT: CA-SWR3_3 callback - OFF !");
      break;
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
      Serial.println("Smart Config fail");
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
      Serial.println("SmartConfig Start");
      digitalWrite(ledB, LOW);
      startSmartConfig();
    }
  }
  else
  {
    buttonActive = false;
  }
}