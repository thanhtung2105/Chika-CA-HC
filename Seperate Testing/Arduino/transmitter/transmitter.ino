#include <RF24.h>
#include <SPI.h>

RF24 radio(9, 10);
const byte data_pipe[6] = "00001";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.printDetails();
  radio.setRetries(15, 15);
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(data_pipe);
}

void loop() {
  char data[] = "Hello world!";
  radio.write(data, strlen(data));
  Serial.println(data);
  delay(1000);
}
