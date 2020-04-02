/* This source code for CA-HC and it has included the functions for controlling list of following products:
        - CA-SWR: {"id":"2b92934f-7a41-4ce1-944d-d33ed6d97e13", "type":"1 button", "RF_channel": 1002502019001}
        - CA-SWR2: {"id":"4a0bfbfe-efff-4bae-927c-c8136df70333", "type":"2 buttons", "RF_channel": 1002502019002}
        - CA-SWR3: {"id":"ebb2464e-ba53-4f22-aa61-c76f24d3343d", "type":"3 buttons", "RF_channel": 1002502019003}
        - CA-SS00: {"id":"f7a3bde5-5a85-470f-9577-cdbf3be121d4", "type":"CA-SS00", "RF_channel": "none"}
        - CA-SS02: {"id":"9d860c55-7899-465b-9fb3-195ae0c0959a", "type":"CA-SS02", "RF_channel": 1002502019004}
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include <RF24.h>
#include <SPI.h>

#define CE 9
#define CSN 53

RF24 radio(CE, CSN);
//Type to determined which kind of CA-SWRx:
const char *CA_SWR = "CA-SWR";
const char *CA_SWR2 = "CA-SWR2";
const char *CA_SWR3 = "CA-SWR3";

const char *CA_SS02 = "CA-SS02";

//RF Channel to communicate with CA-SWRx product:
const uint64_t address_CA_SWR = 1002502019001;
const uint64_t address_CA_SWR2 = 1002502019002;
const uint64_t address_CA_SWR3 = 1002502019003;

const uint64_t address_CA_SS02 = 1002502019004;

boolean control_CA_SWRx[3];   //Control form MQTT
boolean sendState_CA_SWRx[3]; //Send to MQTT

uint32_t data_CA_SS02[3];

uint8_t pipeNum;

void setup()
{
  SPI.begin();
  Serial.begin(9600);
  Serial3.begin(115200); //Open communicate gate with ESP

  radio.begin();
  radio.setRetries(15, 15);
  radio.setPALevel(RF24_PA_MAX);
  radio.printDetails();

  Serial.println("MEGA2560 is ready !");
}

void loop()
{
  //First - Waiting from any control command from MQTT - ESP ?!
  if (Serial3.available())
  {
    String payload_ESP = Serial3.readStringUntil('\r');

    Serial.println(payload_ESP);

    StaticJsonDocument<200> JsonDoc_ESP;
    deserializeJson(JsonDoc_ESP, payload_ESP);
    String type = JsonDoc_ESP["type"];

    boolean button_1 = JsonDoc_ESP["button_1"];
    boolean button_2 = JsonDoc_ESP["button_2"];
    boolean button_3 = JsonDoc_ESP["button_3"];

    boolean mode = JsonDoc_ESP["mode"];
    int delayTime = JsonDoc_ESP["delayTime"];

    radio.stopListening(); //Change mode from Send_stateDevice to Control_device
    if (type == CA_SWR)
    {
      radio.openWritingPipe(address_CA_SWR);
      control_CA_SWRx[0] = button_1;
      radio.write(&control_CA_SWRx, sizeof(control_CA_SWRx));

      Serial.println("Receive control command from CA-SWR: ");
      Serial.print("Button: ");
      Serial.println(button_1);
      Serial.println();
    }
    else if (type == CA_SWR2)
    {
      radio.openWritingPipe(address_CA_SWR2);
      control_CA_SWRx[0] = button_1;
      control_CA_SWRx[1] = button_2;
      radio.write(&control_CA_SWRx, sizeof(control_CA_SWRx));

      Serial.println("Receive control command from CA-SWR2: ");
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

      Serial.println("Receive control command from CA-SWR3: ");
      Serial.print("Button 1: ");
      Serial.println(button_1);
      Serial.print("Button 2: ");
      Serial.println(button_2);
      Serial.print("Button 3: ");
      Serial.println(button_3);
      Serial.println();
    }
    else if (type == CA_SS02)
    {
      radio.openWritingPipe(address_CA_SS02);
      data_CA_SS02[0] = mode;
      data_CA_SS02[1] = delayTime * 1000;
      radio.write(&data_CA_SS02, sizeof(data_CA_SS02));

      Serial.println("Receive control command from CA-SS02: ");
      Serial.print("Mode: ");
      Serial.println(data_CA_SS02[0]);
      Serial.print("New delay time: ");
      Serial.println(data_CA_SS02[1]);
      Serial.println();
    }
  }

  //Second - Send state of device when having anything changes from product:
  radio.openReadingPipe(1, address_CA_SWR);
  radio.openReadingPipe(2, address_CA_SWR2);
  radio.openReadingPipe(3, address_CA_SWR3);
  radio.openReadingPipe(4, address_CA_SS02);
  radio.startListening();

  if (radio.available(&pipeNum))
  {
    Serial.print("Change state device from pipe's device ");
    Serial.println(pipeNum);

    String payload_MEGA;
    StaticJsonDocument<200> JsonDoc_MEGA;
    switch (pipeNum)
    {
    case 1:
      /* Reading Pipe from address of CA_SWR */
      memset(&sendState_CA_SWRx, ' ', sizeof(sendState_CA_SWRx));
      radio.read(&sendState_CA_SWRx, sizeof(sendState_CA_SWRx));

      JsonDoc_MEGA["type"] = "CA-SWR";
      JsonDoc_MEGA["button_1"] = sendState_CA_SWRx[0];
      serializeJson(JsonDoc_MEGA, payload_MEGA);
      Serial3.print(payload_MEGA);
      Serial.println(payload_MEGA);
      break;

    case 2:
      /* Reading Pipe from address of CA_SWR2 */
      memset(&sendState_CA_SWRx, ' ', sizeof(sendState_CA_SWRx));
      radio.read(&sendState_CA_SWRx, sizeof(sendState_CA_SWRx));

      JsonDoc_MEGA["type"] = "CA-SWR2";
      JsonDoc_MEGA["button_1"] = sendState_CA_SWRx[0];
      JsonDoc_MEGA["button_2"] = sendState_CA_SWRx[1];
      serializeJson(JsonDoc_MEGA, payload_MEGA);
      Serial3.print(payload_MEGA);
      Serial.println(payload_MEGA);
      break;

    case 3:
      /* Reading Pipe from address of CA_SWR3 */
      memset(&sendState_CA_SWRx, ' ', sizeof(sendState_CA_SWRx));
      radio.read(&sendState_CA_SWRx, sizeof(sendState_CA_SWRx));

      JsonDoc_MEGA["type"] = "CA-SWR3";
      JsonDoc_MEGA["button_1"] = sendState_CA_SWRx[0];
      JsonDoc_MEGA["button_2"] = sendState_CA_SWRx[1];
      JsonDoc_MEGA["button_3"] = sendState_CA_SWRx[2];
      serializeJson(JsonDoc_MEGA, payload_MEGA);
      Serial3.print(payload_MEGA);
      Serial.println(payload_MEGA);
      break;

    case 4:
      /* Reading Pipe from address of CA_SS02 */
      memset(&data_CA_SS02, ' ', sizeof(data_CA_SS02));
      radio.read(&data_CA_SS02, sizeof(data_CA_SS02));

      JsonDoc_MEGA["type"] = "CA-SS02";
      JsonDoc_MEGA["mode"] = data_CA_SS02[0];
      JsonDoc_MEGA["delayTime"] = data_CA_SS02[1];
      JsonDoc_MEGA["deviceState"] = data_CA_SS02[2];
      serializeJson(JsonDoc_MEGA, payload_MEGA);
      Serial3.print(payload_MEGA);
      Serial.println(payload_MEGA);
      break;

    default:
      break;
    }
  }
}