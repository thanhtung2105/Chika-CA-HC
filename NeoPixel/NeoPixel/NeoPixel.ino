#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN       6
#define NUMPIXELS 8
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int red_value[9] = {255, 255, 255, 0, 0, 75, 128, 255};
int green_value[9] = {0, 165, 255, 128, 0, 0, 0, 255};
int blue_value[9] = {0, 0, 0, 0, 255, 130, 128, 255};

void swap(int & a, int & b) {
  int temp = a;
  a = b;
  b = temp;
}

void changing(int a[]) {
  for (int i = 0; i < 8; i++)
  {
    swap(a[i], a[i+1]);
  }
}

#define DELAYVAL 100

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
#endif
  pixels.begin();
}

uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;
  for (j = 0; j < 256; j++) {
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

void pixels_off()
{
  for (int i = 0; i < 8; i++)
  {
  pixels.setPixelColor(i, 0, 0, 0);
  }
  pixels.show();
}

void Start_Up_1()
{
  for (int i = 0; i < 8; i++)
  {
    pixels.setBrightness(pow(2,i) - 1);
    pixels.setPixelColor(0, red_value[i], green_value[i], blue_value[i]);
    changing(red_value);
    changing(green_value);
    changing(blue_value);
    pixels.setPixelColor(1, red_value[i], green_value[i], blue_value[i]);
    changing(red_value);
    changing(green_value);
    changing(blue_value);
    pixels.setPixelColor(2, red_value[i], green_value[i], blue_value[i]);
    changing(red_value);
    changing(green_value);
    changing(blue_value);
    pixels.setPixelColor(3, red_value[i], green_value[i], blue_value[i]);
    changing(red_value);
    changing(green_value);
    changing(blue_value);
    pixels.setPixelColor(4, red_value[i], green_value[i], blue_value[i]);
    changing(red_value);
    changing(green_value);
    changing(blue_value);
    pixels.setPixelColor(5, red_value[i], green_value[i], blue_value[i]);
    changing(red_value);
    changing(green_value);
    changing(blue_value);
    pixels.setPixelColor(6, red_value[i], green_value[i], blue_value[i]);
    changing(red_value);
    changing(green_value);
    changing(blue_value);
    pixels.setPixelColor(7, red_value[i], green_value[i], blue_value[i]);
    pixels.show();
    delay(100);
  }

  for (int i = 7; i > 0; i--)
  {
    pixels.setBrightness(pow(2,i) - 1);
    pixels.setPixelColor(0, red_value[i], green_value[i], blue_value[i]);
    changing(red_value);
    changing(green_value);
    changing(blue_value);
    pixels.setPixelColor(1, red_value[i], green_value[i], blue_value[i]);
    changing(red_value);
    changing(green_value);
    changing(blue_value);
    pixels.setPixelColor(2, red_value[i], green_value[i], blue_value[i]);
    changing(red_value);
    changing(green_value);
    changing(blue_value);
    pixels.setPixelColor(3, red_value[i], green_value[i], blue_value[i]);
    changing(red_value);
    changing(green_value);
    changing(blue_value);
    pixels.setPixelColor(4, red_value[i], green_value[i], blue_value[i]);
    changing(red_value);
    changing(green_value);
    changing(blue_value);
    pixels.setPixelColor(5, red_value[i], green_value[i], blue_value[i]);
    changing(red_value);
    changing(green_value);
    changing(blue_value);
    pixels.setPixelColor(6, red_value[i], green_value[i], blue_value[i]);
    changing(red_value);
    changing(green_value);
    changing(blue_value);
    pixels.setPixelColor(7, red_value[i], green_value[i], blue_value[i]);
    pixels.show();
    delay(100);
  }
}

void Start_Up_2()
{
  pixels.setPixelColor(7, 0, 0, 255);
  pixels.setPixelColor(6, 0, 0, 255);
  pixels.setPixelColor(5, 0, 0, 255);
  pixels.setPixelColor(4, 0, 0, 0);
  pixels.setPixelColor(3, 0, 0, 0);
  pixels.setPixelColor(2, 0, 255, 0);
  pixels.setPixelColor(1, 0, 255, 0);
  pixels.setPixelColor(0, 255, 0, 0);
  pixels.setBrightness(30);
  pixels.show();
}

void loop() {
  Start_Up_1();
  delay(200);
  pixels_off();
  delay(400);
  Start_Up_2();
  delay(100000);
}
