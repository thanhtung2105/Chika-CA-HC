#include <ArduinoJson.h>

void setup()
{
  Serial.begin(9600);
  Serial3.begin(115200);
  Serial.println("CA-HC_Mega say hello to your home!");
}

void loop()
{
  delay(3000);
  String payload;
  StaticJsonDocument<1000> Testing;
  Testing["type"] = "ebb2464e-ba53-4f22-aa61-c76f24d3343d";
  Testing["button_1"] = "false";
  Testing["button_2"] = "false";
  Testing["button_3"] = "false";
  serializeJson(Testing, payload);
  Serial3.print(payload);
  Serial.println(payload);
}
