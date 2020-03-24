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

//Topic: product_id/button_id             char[38] = 3
const char *CA_SWR = "2b92934f-7a41-4ce1-944d-d33ed6d97e13/7362251b-a856-4ef2-ab9b-33fd27b137a8";
//                                        char[38] = 4/a
const char *CA_SWR2_1 = "4a0bfbfe-efff-4bae-927c-c8136df70333/e4859254-ccd6-400f-abec-a5f74292674e";
const char *CA_SWR2_2 = "4a0bfbfe-efff-4bae-927c-c8136df70333/6a054789-0a32-4807-a2a7-66fd5a4cf967";
//                                        char[38] = f/5/b
const char *CA_SWR3_1 = "ebb2464e-ba53-4f22-aa61-c76f24d3343d/5faf98dd-9aa4-4a02-b0dc-344d5c6304fe";
const char *CA_SWR3_2 = "ebb2464e-ba53-4f22-aa61-c76f24d3343d/9554cca1-0133-4682-81f9-acc8bcb40121";
const char *CA_SWR3_3 = "ebb2464e-ba53-4f22-aa61-c76f24d3343d/7b777605-1ea2-4878-9194-1b1e72edcb98";

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

  WiFi.setAutoConnect(true);   // auto connect when start
  WiFi.setAutoReconnect(true); // auto reconnect the old WiFi when leaving internet

  pinMode(ledR, OUTPUT);      // led red set on
  pinMode(ledB, OUTPUT);      // led blue set on
  pinMode(btn_config, INPUT); // btn_config is ready

  ticker.attach(1, tick2); // initial led show up

  uint16_t i = 0;
  while (!WiFi.isConnected()) // check WiFi is connected
  {
    i++;
    delay(100);
    if (i >= 100) // timeout and break while loop
      break;
  }

  if (!WiFi.isConnected()) // still not connected
  {
    startSmartConfig(); // start Smartconfig
  }
  else
  {
    ticker.detach();         // shutdown ticker
    digitalWrite(ledR, LOW); // show led
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

        StaticJsonDocument<1000> JsonDoc;
        deserializeJson(JsonDoc, payload);

        String type = JsonDoc["type"];
        // float temp = JsonDoc["temperature"];
        // float humi = JsonDoc["humidity"];

        // Serial.print(F("Humidity: "));
        // Serial.print(humi);
        // Serial.print(F("%  Temperature: "));
        // Serial.print(temp);
        // Serial.println(F("°C "));

        // char payload_char[1000];
        // payload.toCharArray(payload_char, payload.length() + 1);
        // client.publish(HCTest, payload_char);
        if (type.charAt(6)=='1')
        {
          boolean btn_1 = JsonDoc["button_data_1"];
          if (btn_1)
          {
            client.publish(CA_SWR, "1", true);
          }
          else
          {
            client.publish(CA_SWR, "0", true);
          }
        }
        else if (type.charAt(6)=='2')
        {
          boolean btn_1 = JsonDoc["button_data_1"];
          boolean btn_2 = JsonDoc["button_data_2"];

          if (btn_1)
          {
            client.publish(CA_SWR2_1, "1", true);
          }
          else
          {
            client.publish(CA_SWR2_1, "0", true);
          }

          if (btn_2)
          {
            client.publish(CA_SWR2_2, "1", true);
          }
          else
          {
            client.publish(CA_SWR2_2, "0", true);
          }
        }
        else
        {
          boolean btn_1 = JsonDoc["button_data_1"];
          boolean btn_2 = JsonDoc["button_data_2"];
          boolean btn_3 = JsonDoc["button_data_3"];

          if (btn_1)
          {
            client.publish(CA_SWR3_1, "1", true);
          }
          else
          {
            client.publish(CA_SWR3_1, "0", true);
          }

          if (btn_2)
          {
            client.publish(CA_SWR3_2, "1", true);
          }
          else
          {
            client.publish(CA_SWR3_2, "0", true);
          }

          if (btn_3)
          {
            client.publish(CA_SWR3_3, "1", true);
          }
          else
          {
            client.publish(CA_SWR3_3, "0", true);
          }
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
      client.subscribe(CA_SWR2_1);
      client.subscribe(CA_SWR2_2);
      client.subscribe(CA_SWR3_1);
      client.subscribe(CA_SWR3_2);
      client.subscribe(CA_SWR3_3);
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
  Serial.print("Topic [");
  Serial.print(topic);
  Serial.print("]: ");
  //Print message of button ID:
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // CA-SWR:
  if ((char)topic[38] == '3')
    switch ((char)payload[0])
    {
    case '1':
      // stateButton_MQTT_CA_SWR[0] = 1;
      // Serial.println("CA_SWR - ON");
      // radio.stopListening();
      // radio.openWritingPipe(address_CA_SWR);
      // radio.write(&stateButton_MQTT_CA_SWR, sizeof(stateButton_MQTT_CA_SWR));
      Serial.println("MQTT: CA-SWR callback - ON !");
      break;
    case '0':
      // stateButton_MQTT_CA_SWR[0] = 0;
      // Serial.println("CA_SWR - OFF");
      // radio.stopListening();
      // radio.openWritingPipe(address_CA_SWR);
      // radio.write(&stateButton_MQTT_CA_SWR, sizeof(stateButton_MQTT_CA_SWR));
      Serial.println("MQTT: CA-SWR callback - OFF !");
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