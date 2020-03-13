#include <Arduino.h>
#include <RF24.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>

/* In this product - the address (channel) to communicate is define as SWR (convert to decimal) 
+ 'date make device' + 'product no.' ; In this case, SWR is known as '83 87 82' and add with the date 
making device for example today is Mar 10th and this is the first product in that day; then the address 
for this SWR is: const byte address[15] = "83878210032001"  ( 83 87 82 | 10 03 20 | 01 )          
83878226022001 (14) - [15]
83878226022002 (14) - [15]
83878226022003 (14) - [15]
*/

Ticker ticker;
RF24 radio(2, 15); //nRF24L01 (CE,CSN) connections PIN
const uint64_t address_CA_SWR = 0xE8E8F0F0A1LL;
const uint64_t address_CA_SWR2 = 0xE8E8F0F0A2LL;
const uint64_t address_CA_SWR3 = 0xE8E8F0F0A3LL;

boolean smartConfigStart = false;

const char *ssid = "username wifi";
const char *password = "password wifi";

boolean stateButton_CA_SWR[1];
boolean stateButton_MQTT_CA_SWR[1];

boolean stateButton_CA_SWR2[2];
boolean stateButton_MQTT_CA_SWR2[2];

boolean stateButton_CA_SWR3[3];
boolean stateButton_MQTT_CA_SWR3[3];

boolean stateButton_received_value[3];

//Topic: product_id/button_id             char[37] = b
const char *CA_SWR = "2a0a6b88-769e-4a63-ac5d-1392a7199e88/be47fa93-15df-44b6-bdba-c821a117cd41";
//                                        char[37] = c/4
const char *CA_SWR2_1 = "da9f8760-13aa-49e2-b881-ffc575ba32f9/cacf2279-e8e5-4f72-801e-0331952767c0";
const char *CA_SWR2_2 = "da9f8760-13aa-49e2-b881-ffc575ba32f9/4ee181da-f292-4f67-bfbd-d9f7a41ebe7e";
//                                        char[37] = 7/5/a
const char *CA_SWR3_1 = "740a8d1e-c649-475e-a270-c5d9a44b40a8/774f2306-51ad-4bf1-ba9e-0ddee9bd2375";
const char *CA_SWR3_2 = "740a8d1e-c649-475e-a270-c5d9a44b40a8/5124ba3a-7a45-472b-8468-6f2a041733ac";
const char *CA_SWR3_3 = "740a8d1e-c649-475e-a270-c5d9a44b40a8/a0087ff7-3613-442f-b6c5-0d5d2f0f1a30";

const int smartConfig_LED = 16;

//Config MQTT broker information:
const char *mqtt_server = "chika.gq";
const int mqtt_port = 2502;
const char *mqtt_user = "chika";
const char *mqtt_pass = "2502";

//Setup MQTT - Wifi ESP12F:
WiFiClient esp_12F;
PubSubClient client(esp_12F);

void setup_Wifi()
{
  delay(100);
  Serial.println();
  Serial.print("Connecting to ... ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/*--------------NEEDED FUNCTIONS--------------*/
void blinking()
{
  bool state = digitalRead(smartConfig_LED);
  digitalWrite(smartConfig_LED, !state);
}

void exitSmartConfig()
{
  WiFi.stopSmartConfig();
  ticker.detach();
}

boolean startSmartConfig()
{
  int t = 0;
  Serial.println("Smart Config Start");
  WiFi.beginSmartConfig();
  delay(500);
  ticker.attach(0.1, blinking);
  while (WiFi.status() != WL_CONNECTED)
  {
    t++;
    Serial.print(".");
    delay(500);
    if (t > 120)
    {
      Serial.println("Smart Config Fail");
      smartConfigStart = false;
      ticker.attach(0.5, blinking);
      delay(3000);
      exitSmartConfig();
      return false;
    }
  }
  smartConfigStart = true;
  Serial.println("WIFI CONNECTED");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.SSID());
  exitSmartConfig();
  return true;
}

void reconnect_mqtt()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    String clientId = "CA-HC_combination - ";
    clientId += String(random(0xffff), HEX);
    Serial.println(clientId);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass))
    {
      Serial.println("Connected");
      client.subscribe(CA_SWR);
      client.subscribe(CA_SWR2_1);
      client.subscribe(CA_SWR2_2);
      client.subscribe(CA_SWR3_1);
      client.subscribe(CA_SWR3_2);
      client.subscribe(CA_SWR3_3);
    }
    else
    {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println("Try again in 1 second");
      delay(1000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  //Topic list test is the value of variables: CA_SWR | CA_SWR2_1 ; CA_SWR2_2 | CA_SWR3_1 ; CA_SWR3_2 ; CA_SWR3_3
  Serial.print("Topic [");
  Serial.print(topic);
  Serial.print("]: ");
  //Print message of button ID:
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // CA-SWR:
  if ((char)topic[37] == 'b')
    switch ((char)payload[0])
    {
    case '1':
      stateButton_MQTT_CA_SWR[0] = 1;
      Serial.println("CA_SWR - ON");
      radio.stopListening();
      radio.openWritingPipe(address_CA_SWR);
      radio.write(&stateButton_MQTT_CA_SWR, sizeof(stateButton_MQTT_CA_SWR));
      break;
    case '0':
      stateButton_MQTT_CA_SWR[0] = 0;
      Serial.println("CA_SWR - OFF");
      radio.stopListening();
      radio.openWritingPipe(address_CA_SWR);
      radio.write(&stateButton_MQTT_CA_SWR, sizeof(stateButton_MQTT_CA_SWR));
      break;
    }

  // CA-SWR2:
  if ((char)topic[37] == 'c')
    switch ((char)payload[0])
    {
    case '1':
      stateButton_MQTT_CA_SWR2[0] = 1;
      Serial.println("CA_SWR2_1 - ON");
      radio.stopListening();
      radio.openWritingPipe(address_CA_SWR2);
      radio.write(&stateButton_MQTT_CA_SWR2, sizeof(stateButton_MQTT_CA_SWR2));
      break;
    case '0':
      stateButton_MQTT_CA_SWR2[0] = 0;
      Serial.println("CA_SWR2_1 - OFF");
      radio.stopListening();
      radio.openWritingPipe(address_CA_SWR2);
      radio.write(&stateButton_MQTT_CA_SWR2, sizeof(stateButton_MQTT_CA_SWR2));
      break;
    }

  if ((char)topic[37] == '4')
    switch ((char)payload[0])
    {
    case '1':
      stateButton_MQTT_CA_SWR2[1] = 1;
      Serial.println("CA_SWR2_2 - ON");
      radio.stopListening();
      radio.openWritingPipe(address_CA_SWR2);
      radio.write(&stateButton_MQTT_CA_SWR2, sizeof(stateButton_MQTT_CA_SWR2));
      break;
    case '0':
      stateButton_MQTT_CA_SWR2[1] = 0;
      Serial.println("CA_SWR2_2 - OFF");
      radio.stopListening();
      radio.openWritingPipe(address_CA_SWR2);
      radio.write(&stateButton_MQTT_CA_SWR2, sizeof(stateButton_MQTT_CA_SWR2));
      break;
    }

  // CA-SWR3:
  if ((char)topic[37] == '7')
    switch ((char)payload[0])
    {
    case '1':
      stateButton_MQTT_CA_SWR3[0] = 1;
      Serial.println("CA_SWR3_1 - ON");
      radio.stopListening();
      radio.openWritingPipe(address_CA_SWR3);
      radio.write(&stateButton_MQTT_CA_SWR3, sizeof(stateButton_MQTT_CA_SWR3));
      break;
    case '0':
      stateButton_MQTT_CA_SWR3[0] = 0;
      Serial.println("CA_SWR3_1 - OFF");
      radio.stopListening();
      radio.openWritingPipe(address_CA_SWR3);
      radio.write(&stateButton_MQTT_CA_SWR3, sizeof(stateButton_MQTT_CA_SWR3));
      break;
    }

  if ((char)topic[37] == '5')
    switch ((char)payload[0])
    {
    case '1':
      stateButton_MQTT_CA_SWR3[1] = 1;
      Serial.println("CA_SWR3_2 - ON");
      radio.stopListening();
      radio.openWritingPipe(address_CA_SWR3);
      radio.write(&stateButton_MQTT_CA_SWR3, sizeof(stateButton_MQTT_CA_SWR3));
      break;
    case '0':
      stateButton_MQTT_CA_SWR3[1] = 0;
      Serial.println("CA_SWR3_2 - OFF");
      radio.stopListening();
      radio.openWritingPipe(address_CA_SWR3);
      radio.write(&stateButton_MQTT_CA_SWR3, sizeof(stateButton_MQTT_CA_SWR3));
      break;
    }

  if ((char)topic[37] == 'a')
    switch ((char)payload[0])
    {
    case '1':
      stateButton_MQTT_CA_SWR3[2] = 1;
      Serial.println("CA_SWR3_3 - ON");
      radio.stopListening();
      radio.openWritingPipe(address_CA_SWR3);
      radio.write(&stateButton_MQTT_CA_SWR3, sizeof(stateButton_MQTT_CA_SWR3));
      break;
    case '0':
      stateButton_MQTT_CA_SWR3[2] = 0;
      Serial.println("CA_SWR3_3 - OFF");
      radio.stopListening();
      radio.openWritingPipe(address_CA_SWR3);
      radio.write(&stateButton_MQTT_CA_SWR3, sizeof(stateButton_MQTT_CA_SWR3));
      break;
    }
}

void setup()
{
  SPI.begin();
  Serial.begin(115200);
  pinMode(smartConfig_LED, OUTPUT);

  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.mode(WIFI_STA);

  //      setup_Wifi();
  delay(6000);

  if (WiFi.status() != WL_CONNECTED)
  {
    startSmartConfig();
  }

  radio.begin();
  radio.setRetries(15, 15);
  radio.setPALevel(RF24_PA_MAX);

  Serial.println("WIFI CONNECTED");
  Serial.println(WiFi.SSID());
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  Serial.println("\nCA-HC-combination say hello to your home <3 !");

  Serial.println("Trying connect MQTT ...");
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  radio.printDetails();
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!client.connected())
    {
      reconnect_mqtt();
    }
    else
      client.loop();
  }

  uint8_t pipeNum;
  radio.openReadingPipe(1, address_CA_SWR);
  radio.openReadingPipe(2, address_CA_SWR2);
  radio.openReadingPipe(3, address_CA_SWR3);
  radio.startListening();

  if (radio.available(&pipeNum))
  {
    Serial.print("Control from CA-SWR");
    Serial.println(pipeNum);

    switch (pipeNum)
    {
    case 1:
      /* Reading Pipe from address of CA_SWR */
      memset(&stateButton_CA_SWR, ' ', sizeof(stateButton_CA_SWR));
      radio.read(&stateButton_CA_SWR, sizeof(stateButton_CA_SWR));
      stateButton_MQTT_CA_SWR[0] = stateButton_CA_SWR[0];
      if (stateButton_MQTT_CA_SWR[0])
        client.publish(CA_SWR, "1", true);
      else
        client.publish(CA_SWR, "0", true);
      break;

    case 2:
      /* Reading Pipe from address of CA_SWR2 */
      memset(&stateButton_CA_SWR2, ' ', sizeof(stateButton_CA_SWR2));
      radio.read(&stateButton_CA_SWR2, sizeof(stateButton_CA_SWR2));
      stateButton_MQTT_CA_SWR2[0] = stateButton_CA_SWR2[0];
      stateButton_MQTT_CA_SWR2[1] = stateButton_CA_SWR2[1];
      if (stateButton_MQTT_CA_SWR2[0])
        client.publish(CA_SWR2_1, "1", true);
      else
        client.publish(CA_SWR2_1, "0", true);

      if (stateButton_MQTT_CA_SWR2[1])
        client.publish(CA_SWR2_2, "1", true);
      else
        client.publish(CA_SWR2_2, "0", true);
      break;

    case 3:
      /* Reading Pipe from address of CA_SWR3 */
      memset(&stateButton_CA_SWR3, ' ', sizeof(stateButton_CA_SWR3));
      radio.read(&stateButton_CA_SWR3, sizeof(stateButton_CA_SWR3));
      stateButton_MQTT_CA_SWR3[0] = stateButton_CA_SWR3[0];
      stateButton_MQTT_CA_SWR3[1] = stateButton_CA_SWR3[1];
      stateButton_MQTT_CA_SWR3[2] = stateButton_CA_SWR3[2];
      if (stateButton_MQTT_CA_SWR3[0])
        client.publish(CA_SWR3_1, "1", true);
      else
        client.publish(CA_SWR3_1, "0", true);

      if (stateButton_MQTT_CA_SWR3[1])
        client.publish(CA_SWR3_2, "1", true);
      else
        client.publish(CA_SWR3_2, "0", true);

      if (stateButton_MQTT_CA_SWR3[2])
        client.publish(CA_SWR3_3, "1", true);
      else
        client.publish(CA_SWR3_3, "0", true);
      break;

    default:
      break;
    }
  }
}