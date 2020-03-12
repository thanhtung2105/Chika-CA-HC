#include <iostream>
#include <RF24/RF24.h>

RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);
const uint8_t data_pipe[6] = "00001";

void setup(void) {
  radio.begin();
  radio.setRetries(15, 15);
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(data_pipe);
}

int main(int argc, char** argv) {
  setup();

  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <message to send>";
    return -1;
  }

  char* data = argv[1];
  radio.write(data, strlen(data) + 1);
  std::cout << "Data Sent" << std::endl;
}
