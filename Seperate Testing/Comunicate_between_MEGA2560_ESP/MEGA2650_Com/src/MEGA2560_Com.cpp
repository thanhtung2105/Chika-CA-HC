#include <Arduino.h>
#include <ArduinoJson.h>
#include <RF24.h>
#include <SPI.h>

#define CE 9
#define CSN 53

RF24 radio(CE, CSN);
const char *CA_SWR = "CA-SWR";
const char *CA_SWR2 = "CA-SWR2";
const char *CA_SWR3 = "CA-SWR3";

const uint64_t address_CA_SWR = 1002502019001;
const uint64_t address_CA_SWR2 = 1002502019002;
const uint64_t address_CA_SWR3 = 1002502019003;

boolean control_CA_SWRx[3];

void setup()
{
  SPI.begin();
  Serial.begin(9600);
  Serial3.begin(115200);

  radio.begin();
  radio.setRetries(15, 15);
  radio.setPALevel(RF24_PA_MAX);

  Serial.println("MEGA2560 Oke!");
  radio.printDetails();
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

    StaticJsonDocument<200> JsonDoc;
    deserializeJson(JsonDoc, payload);
    String type = JsonDoc["type"];
    boolean button_1 = JsonDoc["button_1"];
    boolean button_2 = JsonDoc["button_2"];
    boolean button_3 = JsonDoc["button_3"];

    if (type == CA_SWR)
    {
      radio.openWritingPipe(address_CA_SWR);
      control_CA_SWRx[0] = button_1;
      radio.write(&control_CA_SWRx, sizeof(control_CA_SWRx));

      Serial.println("Receive data from CA-SWR: ");
      Serial.print("Button 1: ");
      Serial.println(button_1);
      Serial.println();
    }
    else if (type == CA_SWR2)
    {
      radio.openWritingPipe(address_CA_SWR2);
      control_CA_SWRx[0] = button_1;
      control_CA_SWRx[1] = button_2;
      radio.write(&control_CA_SWRx, sizeof(control_CA_SWRx));

      Serial.println("Receive data from CA-SWR2: ");
      Serial.print("Button 1: ");
      Serial.println(button_1);
      Serial.print("Button 2: ");
      Serial.println(button_2);
      Serial.println();
    }
    else if (type == CA_SWR3)
    {
      radio.openWritingPipe(address_CA_SWR3);
      control_CA_SWRx[0] = button_1;
      control_CA_SWRx[1] = button_2;
      control_CA_SWRx[2] = button_3;
      radio.write(&control_CA_SWRx, sizeof(control_CA_SWRx));
  
      Serial.println("Receive data from CA-SWR3: ");
      Serial.print("Button 1: ");
      Serial.println(button_1);
      Serial.print("Button 2: ");
      Serial.println(button_2);
      Serial.print("Button 3: ");
      Serial.println(button_3);
      Serial.println();
    }
  }
}