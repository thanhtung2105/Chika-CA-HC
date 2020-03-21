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

	Product code: 4a0bfbfe-efff-4bae-927c-c8136df70333
	RF channel (2 button): 83878226022002
	Button topic: e4859254-ccd6-400f-abec-a5f74292674e
                6a054789-0a32-4807-a2a7-66fd5a4cf967
*/

Ticker ticker;
RF24 radio(2, 15); //nRF24L01 (CE,CSN) connections PIN
const uint64_t address = 1002502019002; //Changeable
boolean smartConfigStart = false;

const char *ssid = "username wifi";
const char *password = "password wifi";

boolean stateButton[2];
boolean stateButton_MQTT[2];

//Topic: product_id/button_id     topic[37] = e/6
const char *CA_SWR_1 = "4a0bfbfe-efff-4bae-927c-c8136df70333/e4859254-ccd6-400f-abec-a5f74292674e";
const char *CA_SWR_2 = "4a0bfbfe-efff-4bae-927c-c8136df70333/6a054789-0a32-4807-a2a7-66fd5a4cf967";
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
    String clientId = "CA-SWR2 - ";
    clientId += String(random(0xffff), HEX);
    Serial.println(clientId);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass))
    {
      Serial.println("Connected");
      client.subscribe(CA_SWR_1);
      client.subscribe(CA_SWR_2);
    }
    else
    {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println("Try again in 3 seconds");
      delay(3000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  //Topic list test is the value of variables: CA_SWR_1 & CA_SWR_2
  Serial.print("Topic [");
  Serial.print(topic);
  Serial.print("]: ");
  //Print message of button ID:
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)topic[37] == 'e')
    switch ((char)payload[0])
    {
    case '1':
      stateButton_MQTT[0] = 1;

      radio.stopListening();
      radio.openWritingPipe(address);
      radio.write(&stateButton_MQTT, sizeof(stateButton_MQTT));
      break;
    case '0':
      stateButton_MQTT[0] = 0;

      radio.stopListening();
      radio.openWritingPipe(address);
      radio.write(&stateButton_MQTT, sizeof(stateButton_MQTT));
      break;
    }

  if ((char)topic[37] == '6')
    switch ((char)payload[0])
    {
    case '1':
      stateButton_MQTT[1] = 1;

      radio.stopListening();
      radio.openWritingPipe(address);
      radio.write(&stateButton_MQTT, sizeof(stateButton_MQTT));
      break;
    case '0':
      stateButton_MQTT[1] = 0;

      radio.stopListening();
      radio.openWritingPipe(address);
      radio.write(&stateButton_MQTT, sizeof(stateButton_MQTT));
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
  delay(8000);

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

  Serial.println("\nCA-HC-2 say hello to your home <3 !");

  Serial.println("Trying connect MQTT ...");
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
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

  radio.openReadingPipe(1, address);
  radio.startListening();

  if (radio.available())
  {
    memset(&stateButton, ' ', sizeof(stateButton));
    radio.read(&stateButton, sizeof(stateButton));

    if (stateButton[0])
      client.publish(CA_SWR_1, "1", true);
    else
      client.publish(CA_SWR_1, "0", true);

    if (stateButton[1])
      client.publish(CA_SWR_2, "1", true);
    else
      client.publish(CA_SWR_2, "0", true);

    stateButton_MQTT[0] = stateButton[0];
    stateButton_MQTT[1] = stateButton[1];
  }
}