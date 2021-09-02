#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include "credentials.h"
#include <Wire.h>

const int PIN_VBAT = 32; // can use pins 32 to 39

const float VL = 3.11; // max value of ADC input
const float VH = 3.33; // max range of input voltage

const char *host = "voltimeter";
WiFiClient espClient;

PubSubClient bcastClient(espClient);

void publish(const char *topic, const char *msg)
{
  Serial.println(msg);
  bcastClient.publish(topic, msg);
}
void publish(const char *msg)
{
  Serial.println(msg);
  bcastClient.publish(mqttDebugTopic, msg);
}

void setupWifiConnection()
{

  WiFi.begin(mySSID, myPW);

  Serial.println("Connecting to WiFi..");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
  }

  const char *s = "Wifi connection stablished with known ip: ";
  String str(s);
  str.concat(WiFi.localIP().toString());

  Serial.println("Connected to the WiFi network");
}

void setupMqttClient()
{
  bcastClient.setServer(mqttServer, mqttPort);

  while (!bcastClient.connected())
  {
    Serial.println("Connecting to MQTT...");

    if (bcastClient.connect("ESP32Client", mqttUser, mqttPassword))
    {
      Serial.println("connected");
    }
    else
    {

      Serial.print("failed with state ");
      Serial.print(bcastClient.state());
      delay(2000);
    }
  }

  bcastClient.publish(mqttDebugTopic, "Subscribed and callback set");
}

void setup()
{
  Serial.begin(115200);
  setupWifiConnection();
  setupMqttClient();
  publish("I2C scanner. Scanning ...");
  byte count = 0;

  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0)
    {
      publish("Found address: ");
      publish(String(DEC).c_str());
      publish(")");
      count++;
      delay(1);
    } // end of good response
  }   // end of for loop
  publish("Done.");
  publish("Found ");
  publish(String(DEC).c_str());
  publish(" device(s).");
}

void loop()
{
}
