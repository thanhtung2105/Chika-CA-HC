#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6
#define NUMPIXELS 8
#define DELAYVAL 100
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//Adafruit_NeoPixel - FS - First Start:
int FS_red[9] = {255, 255, 255, 0, 0, 75, 128, 255};
int FS_green[9] = {0, 165, 255, 128, 0, 0, 0, 255};
int FS_blue[9] = {0, 0, 0, 0, 255, 130, 128, 255};

//Adafruit_NeoPixel - CS - Chika Start:
int CS_red[9] = {65, 0, 0, 0, 65, 0, 0, 0};
int CS_green[9] = {40, 60, 255, 128, 40, 60, 255, 128};
int CS_blue[9] = {134, 255, 153, 0, 134, 255, 153, 0};

//================================= Adafruit_NeoPixel Start Up - BEGIN RANGE =================================//
void swap(int &a, int &b)
{
  int temp = a;
  a = b;
  b = temp;
}

// Right to Left
void shiftLeft(int a[])
{
  for (int i = 0; i < 8; i++)
  {
    swap(a[i], a[i + 1]);
  }
}

// Left to Right
void shiftRight(int a[])
{
  for (int i = 8; i > 0; i--)
  {
    swap(a[i], a[i - 1]);
  }
}

void pixelsOff()
{
  for (int i = 0; i < 8; i++)
  {
    pixels.setPixelColor(i, 0, 0, 0);
  }
  pixels.show();
}

void FirstStartUp()
{
  for (int i = 0; i < 8; i++)
  {
    pixels.setBrightness(pow(2, i) - 1);
    for (int j = 0; j < 7; j++)
    {
      pixels.setPixelColor(j, FS_red[i], FS_green[i], FS_blue[i]);
      shiftLeft(FS_red);
      shiftLeft(FS_green);
      shiftLeft(FS_blue);
    }
    pixels.setPixelColor(7, FS_red[i], FS_green[i], FS_blue[i]);
    pixels.show();
    delay(80);
  }

  for (int i = 7; i > 0; i--)
  {
    pixels.setBrightness(pow(2, i) - 1);
    for (int j = 0; j < 7; j++)
    {
      pixels.setPixelColor(j, FS_red[i], FS_green[i], FS_blue[i]);
      shiftLeft(FS_red);
      shiftLeft(FS_green);
      shiftLeft(FS_blue);
    }
    pixels.setPixelColor(7, FS_red[i], FS_green[i], FS_blue[i]);
    pixels.show();
    delay(80);
  }
}

void ChikaStartUp()
{
  for (int i = 0; i < 8; i++)
  {
    pixels.setBrightness(pow(2, i) - 1);
    for (int j = 0; j < 7; j++)
    {
      pixels.setPixelColor(j, CS_red[i], CS_green[i], CS_blue[i]);
      shiftRight(CS_red);
      shiftRight(CS_green);
      shiftRight(CS_blue);
    }
    pixels.setPixelColor(7, CS_red[i], CS_green[i], CS_blue[i]);
    pixels.show();
    delay(100);
  }

  for (int i = 7; i > 0; i--)
  {
    pixels.setBrightness(pow(2, i) - 1);
    for (int j = 0; j < 7; j++)
    {
      pixels.setPixelColor(j, CS_red[i], CS_green[i], CS_blue[i]);
      shiftRight(CS_red);
      shiftRight(CS_green);
      shiftRight(CS_blue);
    }
    pixels.setPixelColor(7, CS_red[i], CS_green[i], CS_blue[i]);
    pixels.show();
    delay(100);
  }
}

void StartUp()
{
  FirstStartUp();
  delay(50);
  ChikaStartUp();
  delay(200);
  pixelsOff();
  delay(500);
}
//================================= Adafruit_NeoPixel Start Up - END RANGE =================================//



void normalMode()
{
  pixels.setPixelColor(7, 0, 0, 255);
  pixels.setPixelColor(6, 0, 0, 255);
  pixels.setPixelColor(5, 0, 0, 255);
  pixels.setPixelColor(4, 0, 0, 255);
  pixels.setPixelColor(3, 0, 0, 255);
  pixels.setPixelColor(2, 0, 255, 0);
  pixels.setPixelColor(1, 0, 255, 0);
  pixels.setPixelColor(0, 255, 0, 0);
  pixels.setBrightness(30);
  pixels.show();
}

void setup()
{
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
#endif
  pixels.begin();
  StartUp();
}

void loop()
{
  normalMode();
  delay(100000);
}
