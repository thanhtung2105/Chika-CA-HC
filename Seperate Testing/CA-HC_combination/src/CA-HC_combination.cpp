#include <Arduino.h>
#include <RF24.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>

/* In this product - the address (channel) to communicate is define as <the HC code> (3 degits)
+ <company code> (7 degits) + <product code> (3 degits). So we have the following list product code:      
CA-SWR: 1002502019001 (13)
CA-SWR2: 1002502019002 (13)
CA-SWR3: 1002502019003 (13)
*/

Ticker ticker;
RF24 radio(2, 15); //nRF24L01 (CE,CSN) connections PIN
const uint64_t address_CA_SWR = 1002502019001;
const uint64_t address_CA_SWR2 = 1002502019002;
const uint64_t address_CA_SWR3 = 1002502019003;

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

//Topic: product_id/button_id             char[38] = 3
const char *CA_SWR = "2b92934f-7a41-4ce1-944d-d33ed6d97e13/7362251b-a856-4ef2-ab9b-33fd27b137a8";
//                                        char[38] = 4/a
const char *CA_SWR2_1 = "4a0bfbfe-efff-4bae-927c-c8136df70333/e4859254-ccd6-400f-abec-a5f74292674e";
const char *CA_SWR2_2 = "4a0bfbfe-efff-4bae-927c-c8136df70333/6a054789-0a32-4807-a2a7-66fd5a4cf967";
//                                        char[38] = f/5/b
const char *CA_SWR3_1 = "ebb2464e-ba53-4f22-aa61-c76f24d3343d/5faf98dd-9aa4-4a02-b0dc-344d5c6304fe";
const char *CA_SWR3_2 = "ebb2464e-ba53-4f22-aa61-c76f24d3343d/9554cca1-0133-4682-81f9-acc8bcb40121";
const char *CA_SWR3_3 = "ebb2464e-ba53-4f22-aa61-c76f24d3343d/7b777605-1ea2-4878-9194-1b1e72edcb98";

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
  if ((char)topic[38] == '3')
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
  if ((char)topic[37] == '4')
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

  if ((char)topic[37] == 'a')
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
  if ((char)topic[37] == 'f')
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

  if ((char)topic[37] == 'b')
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