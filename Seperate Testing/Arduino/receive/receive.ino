#include <RF24.h>
#include <SPI.h>
#include "printf.h"

RF24 radio(9, 10);
const byte data_pipe[6] = "00001";

void setup() {
  Serial.begin(9600);
  printf_begin();
  
  radio.begin();
  radio.setRetries(15, 15);
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(1, data_pipe);
  radio.startListening();
}

void loop() {
  if (radio.available())
  {
    int payload_size = radio.getDynamicPayloadSize();
    if (payload_size > 1)
    {
      char* payload = new char[payload_size + 1];
      radio.read(payload, payload_size);
      payload[payload_size] = '\0';
      printf("Got Message: %s\r\n", payload);
    }
  }
}
