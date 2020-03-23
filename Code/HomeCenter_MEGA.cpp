#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define CE 9
#define CSN 53
#define dht_pin A0
#define dht_type DHT22

DHT dht(dht_pin, dht_type);

RF24 radio(CE,CSN);

const uint64_t address = 0xE8E8F0F0A6LL;

float dhtValue[3];

void setup() {
    Serial.begin(9600);
    Serial3.begin(115200);
    Serial.println("Serial Mega ready");
    SPI.begin();
    //========================RF========================
    radio.begin();
    radio.setRetries(15,15);
    radio.setPALevel(RF24_PA_MAX);
    radio.openWritingPipe(address);
    //==================================================
    dht.begin();
}

void loop() {

    delay(2000);
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

    StaticJsonDocument<1000> JsonDoc;
    JsonDoc["type"] = "DHT";
    JsonDoc["temperature"] = t;
    JsonDoc["humidity"] = h;

    String payload;
    serializeJson(JsonDoc, payload);
    Serial.println(payload);
    Serial3.print(payload);
}