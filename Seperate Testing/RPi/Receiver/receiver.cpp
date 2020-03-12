#include <iostream>
#include <RF24/RF24.h>

RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);
const uint8_t data_pipe[6] = "00001";

void setup(void) {
  radio.begin();
  radio.setRetries(15, 15);
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(1, data_pipe);
  radio.startListening();
}

int main(int argc, char** argv) {
  setup();

  while (true) {
    if (radio.available()) {
      int payload_size = radio.getDynamicPayloadSize();
      if (payload_size > 1) {
        char* payload = new char[payload_size + 1];
        radio.read(payload, payload_size);
        payload[payload_size] = '\0';
        std::cout << "Got Message: " << payload << std::endl;
      }
    }
  }
}
