#include <SPI.h>
#include <RF24.h>

RF24 radio(9, 10);
const byte address[6] = "00001";

//int ldrSensor = A7;
//int flameSensor = A6;
//int rainSensor = A5;
int value[3];

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setRetries(15, 15);
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(address);
}

void loop() {
//  value[0] = analogRead(ldrSensor);
//  value[1] = analogRead(flameSensor);
//  value[2] = analogRead(rainSensor);

    value[0] = 69;
//  value[1] = 9;
//  value[2] = 3;

  radio.write(&value, sizeof(value));

  Serial.print(value[0]);
//  Serial.print("\t");
//  Serial.print(value[1]);
//  Serial.print("\t");
//  Serial.println(value[2]);
  delay(100);
}
