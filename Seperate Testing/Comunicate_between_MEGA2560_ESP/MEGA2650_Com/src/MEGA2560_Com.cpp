#include <Arduino.h>
#include <ArduinoJson.h>

void setup()
{
  Serial.begin(9600);
  Serial3.begin(115200);
  Serial.println("MEGA2560 Oke!");
}

void loop()
{
  // delay(3000);
  // String payload;
  // StaticJsonDocument<1000> Testing;
  // Testing["type"] = "ebb2464e-ba53-4f22-aa61-c76f24d3343d";
  // Testing["button_1"] = "false";
  // Testing["button_2"] = "false";
  // Testing["button_3"] = "false";
  // serializeJson(Testing, payload);
  // Serial3.print(payload);
  // Serial.println(payload);
  if (Serial3.available())
  {
    String payload = Serial3.readStringUntil('\r');

    if (payload == "abc")
    {
      Serial.println("Da thay abc");
    }

    if (payload == "\n123")
    {
      Serial.println("Da thay 123");
    }
  }
}