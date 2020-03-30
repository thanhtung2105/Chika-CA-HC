#include <Arduino.h>
#include <ArduinoJson.h>

void setup()
{
  Serial.begin(115200);
  Serial.println("ESP12E Oke!");
}

void loop()
{
  // if (Serial.available())
  // {
  //   String payload = Serial.readString();
  //   Serial.println(payload);
  // }

  delay(3000);
  String payload = "abc";
  // StaticJsonDocument<1000> Testing;
  // Testing["type"] = "ebb2464e-ba53-4f22-aa61-c76f24d3343d";
  // Testing["button_1"] = "false";
  // Testing["button_2"] = "false";
  // Testing["button_3"] = "false";
  // serializeJson(Testing, payload);
  Serial.print(payload);
  Serial.println();
  Serial.print("123");
}