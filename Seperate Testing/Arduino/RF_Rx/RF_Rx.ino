#include <SPI.h>
#include <RF24.h>
#include "printf.h"

RF24 radio(9, 10); //ce,cs pin
const byte address[6] = "00001";
int value[3];

void setup() {
  Serial.begin(9600);
  printf_begin();
  
  radio.begin();
  radio.setRetries(15, 15);
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(1, address);
  radio.startListening();
}

void loop() {
  if (radio.available())
  {
     int payload_size = radio.getDynamicPayloadSize();
     if (payload_size > 1)
     {
  
    while (radio.available()) {
      memset(&value, ' ', sizeof(value));
      radio.read(&value, sizeof(value));
      Serial.print(value[0]);
      Serial.print("\t");
      Serial.print(value[1]);
      Serial.print("\t");
      Serial.println(value[2]);
      delay(1000);
    
}
     }
  }
}
