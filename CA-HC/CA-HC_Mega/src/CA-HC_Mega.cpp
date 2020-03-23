#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
//#include <Adafruit_Sensor.h>
//#include <DHT.h>
#include <ArduinoJson.h>

#define CE 9
#define CSN 53
// #define dht_pin A0
// #define dht_type DHT22

/* In this product - the address (channel) to communicate is define as <the HC code> (3 degits)
+ <company code> (7 degits) + <product code> (3 degits). So we have the following list product code:      
CA-SWR: 1002502019001 (13)
CA-SWR2: 1002502019002 (13)
CA-SWR3: 1002502019003 (13)
*/

//DHT dht(dht_pin, dht_type);
RF24 radio(CE, CSN);

const uint64_t address_CA_SWR = 1002502019001;
const uint64_t address_CA_SWR2 = 1002502019002;
const uint64_t address_CA_SWR3 = 1002502019003;

boolean stateButton_CA_SWR[1];
boolean stateButton_MQTT_CA_SWR[1];

boolean stateButton_CA_SWR2[2];
boolean stateButton_MQTT_CA_SWR2[2];

boolean stateButton_CA_SWR3[3];
boolean stateButton_MQTT_CA_SWR3[3];

boolean stateButton_received_value[3];

//float dhtValue[3];

void setup()
{
    Serial.begin(9600);
    Serial3.begin(115200);
    Serial.println("CA-HC_Mega is ready!");
    SPI.begin();

    //========================RF========================
    radio.begin();
    radio.setRetries(15, 15);
    radio.setPALevel(RF24_PA_MAX);
    radio.printDetails();

    //=======================DHT========================
    //    dht.begin();
}

void loop()
{
    /*
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t))
    {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
    }

    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.println(F("°C "));

    // dhtValue[0] = t;
    // dhtValue[1] = h;
    */

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
        StaticJsonDocument<1000> Json_CA_SWR;
        StaticJsonDocument<1000> Json_CA_SWR2;
        StaticJsonDocument<1000> Json_CA_SWR3;
        switch (pipeNum)
        {
        case 1:
            /* Reading Pipe from address of CA_SWR */
            memset(&stateButton_CA_SWR, ' ', sizeof(stateButton_CA_SWR));
            radio.read(&stateButton_CA_SWR, sizeof(stateButton_CA_SWR));
            stateButton_MQTT_CA_SWR[0] = stateButton_CA_SWR[0];
            
            Json_CA_SWR["type"] = "CA-SWR";
            Json_CA_SWR["button_data"] = stateButton_MQTT_CA_SWR[0];
            serializeJson(Json_CA_SWR, payload);
            Serial3.print(payload);
            Serial.println(payload);
            break;

        case 2:
            /* Reading Pipe from address of CA_SWR2 */
            memset(&stateButton_CA_SWR2, ' ', sizeof(stateButton_CA_SWR2));
            radio.read(&stateButton_CA_SWR2, sizeof(stateButton_CA_SWR2));
            stateButton_MQTT_CA_SWR2[0] = stateButton_CA_SWR2[0];
            stateButton_MQTT_CA_SWR2[1] = stateButton_CA_SWR2[1];
            
            Json_CA_SWR2["type"] = "CA-SWR2";
            Json_CA_SWR2["button_data_1"] = stateButton_MQTT_CA_SWR2[0];
            Json_CA_SWR2["button_data_2"] = stateButton_MQTT_CA_SWR2[1];
            serializeJson(Json_CA_SWR2, payload);
            Serial3.print(payload);
            Serial.println(payload);
            break;

        case 3:
            /* Reading Pipe from address of CA_SWR3 */
            memset(&stateButton_CA_SWR3, ' ', sizeof(stateButton_CA_SWR3));
            radio.read(&stateButton_CA_SWR3, sizeof(stateButton_CA_SWR3));
            stateButton_MQTT_CA_SWR3[0] = stateButton_CA_SWR3[0];
            stateButton_MQTT_CA_SWR3[1] = stateButton_CA_SWR3[1];
            stateButton_MQTT_CA_SWR3[2] = stateButton_CA_SWR3[2];
            
            Json_CA_SWR3["type"] = "CA-SWR2";
            Json_CA_SWR3["button_data_1"] = stateButton_MQTT_CA_SWR3[0];
            Json_CA_SWR3["button_data_2"] = stateButton_MQTT_CA_SWR3[1];
            Json_CA_SWR3["button_data_3"] = stateButton_MQTT_CA_SWR3[2];
            serializeJson(Json_CA_SWR3, payload);
            Serial3.print(payload);
            Serial.println(payload);
            break;

        default:
            break;
        }
    }

    // StaticJsonDocument<1000> JsonDoc;
    // JsonDoc["type"] = "DHT";
    // JsonDoc["temperature"] = t;
    // JsonDoc["humidity"] = h;

    // String payload;
    // serializeJson(JsonDoc, payload);
    // Serial.println(payload);
    // Serial3.print(payload);
}