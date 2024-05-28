#include <Arduino.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

// MQTT Broker
const char *mqtt_broker = "51.20.105.27"; //
const char *topic = "mqtt/esp32";
const char *subtopic1 = "mqtt/receive";
const char *subtopic2 = "mqtt/lcd";
const char *mqtt_username = "userttpu";
const char *mqtt_password = "studentpass";
const int mqtt_port = 1884;

WiFiClient espClient;
PubSubClient client(espClient);

//===========PINS==================
#define relayPIN 25
#define buttonPin 35
#define pirMotion 32
#define bOut 12
#define rOut 14



void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void setup()
{
  Serial.begin(115200);
  delay(10);

  WiFiManager wifiManager;

  if (!wifiManager.autoConnect("esp32noCam", "esp32nocam"))
  {
    Serial.println("failed to connect and hit timeout");
    // reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);

    // connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected())
    {
      String client_id = "esp32-client";
      Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
      {
        Serial.println("Public EMQX MQTT broker connected");
      }
      else
      {
        Serial.print("failed with state ");
        Serial.print(client.state());
        delay(2000);
      }
    }
    // Publish and subscribe
    client.subscribe(subtopic1);
    client.subscribe(subtopic2);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(relayPIN, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(pirMotion, INPUT);
  pinMode(bOut, OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
}