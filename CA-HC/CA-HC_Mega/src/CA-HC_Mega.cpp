#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <ArduinoJson.h>

#define CE 9
#define CSN 53

RF24 radio(CE, CSN);

const uint64_t address_CA_SWR = 1002502019001;
const uint64_t address_CA_SWR2 = 1002502019002;
const uint64_t address_CA_SWR3 = 1002502019003;

boolean sendState_CA_SWRx[3];

void setup()
{
    Serial.begin(9600);
    Serial3.begin(115200);
    Serial.println("CA-HC_Mega is ready!");
    SPI.begin();

    radio.begin();
    radio.setRetries(15, 15);
    radio.setPALevel(RF24_PA_MAX);
    radio.printDetails();
}

void loop()
{
    uint8_t pipeNum;
    radio.openReadingPipe(1, address_CA_SWR);
    radio.openReadingPipe(2, address_CA_SWR2);
    radio.openReadingPipe(3, address_CA_SWR3);
    radio.startListening();

    if (radio.available(&pipeNum))
    {
        Serial.print("Control from CA-SWR");
        Serial.println(pipeNum);

        String payload;
        StaticJsonDocument<200> Json_CA_SWR;
        StaticJsonDocument<200> Json_CA_SWR2;
        StaticJsonDocument<200> Json_CA_SWR3;
        switch (pipeNum)
        {
        case 1:
            /* Reading Pipe from address of CA_SWR */
            memset(&sendState_CA_SWRx, ' ', sizeof(sendState_CA_SWRx));
            radio.read(&sendState_CA_SWRx, sizeof(sendState_CA_SWRx));

            Json_CA_SWR["type"] = "CA-SWR";
            Json_CA_SWR["button_1"] = sendState_CA_SWRx[0];
            serializeJson(Json_CA_SWR, payload);
            Serial3.print(payload);
            Serial.println(payload);
            break;

        case 2:
            /* Reading Pipe from address of CA_SWR2 */
            memset(&sendState_CA_SWRx, ' ', sizeof(sendState_CA_SWRx));
            radio.read(&sendState_CA_SWRx, sizeof(sendState_CA_SWRx));

            Json_CA_SWR2["type"] = "CA-SWR2";
            Json_CA_SWR2["button_1"] = sendState_CA_SWRx[0];
            Json_CA_SWR2["button_2"] = sendState_CA_SWRx[1];
            serializeJson(Json_CA_SWR2, payload);
            Serial3.print(payload);
            Serial.println(payload);
            break;

        case 3:
            /* Reading Pipe from address of CA_SWR3 */
            memset(&sendState_CA_SWRx, ' ', sizeof(sendState_CA_SWRx));
            radio.read(&sendState_CA_SWRx, sizeof(sendState_CA_SWRx));

            Json_CA_SWR3["type"] = "CA-SWR3";
            Json_CA_SWR3["button_1"] = sendState_CA_SWRx[0];
            Json_CA_SWR3["button_2"] = sendState_CA_SWRx[1];
            Json_CA_SWR3["button_3"] = sendState_CA_SWRx[2];
            serializeJson(Json_CA_SWR3, payload);
            Serial3.print(payload);
            Serial.println(payload);
            break;

        default:
            break;
        }
    }
}