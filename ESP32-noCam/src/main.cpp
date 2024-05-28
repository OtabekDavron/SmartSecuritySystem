#include <Arduino.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_AHTX0.h>
#include "ThingSpeak.h"
#include <WiFi.h>

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

//thingspeak
WiFiClient  client1;

unsigned long myChannelNumber = 2;
const char * myWriteAPIKey = "xxxxxxxxxxxxxxxx";

// LCD
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

// AHT10
Adafruit_AHTX0 aht;

//===========PINS==================
#define relayPIN 25
#define buttonPin 35
#define pirMotion 32
#define bOut 12
#define rOut 14

//======variables==========
String buffer = "";
bool doorFlag = 0;
unsigned long ttime;
bool lcdFlag = 0;
unsigned long ttimeLcd;
unsigned long lastTime = 110000;

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  buffer = "";
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    buffer += (char)payload[i];
  }

  Serial.println();
  Serial.println("-----------------------");
  StaticJsonDocument<200> doc;
  if (strcmp(topic, subtopic1) == 0)
  {

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, buffer);

    // Test if parsing succeeds
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    // Extract values
    String door = doc["door"];

    // Print values
    Serial.println(door);

    if (door.equals("open"))
    {
      doorFlag = 1;
      digitalWrite(relayPIN, 1);
      ttime = millis();
    }
  }
  else if (strcmp(topic, subtopic2) == 0)
  {

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, buffer);

    // Test if parsing succeeds
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    // Extract values
    const char *msg = doc["msg"];

    // Print values
    Serial.println(msg);
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print(String(msg));
    ttimeLcd = millis();
    lcdFlag = 1;
  }
}

void openDoor()
{
  if (doorFlag and millis() - ttime >= 5000)
  {
    doorFlag = 0;
    digitalWrite(relayPIN, 0);
  }
}
void thing_speak()
{
  if ((millis() - lastTime) > 120000)
  {

    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data
    Serial.print("Temperature: ");
    Serial.print(temp.temperature);
    Serial.println(" degrees C");
    Serial.print("Humidity: ");
    Serial.print(humidity.relative_humidity);
    Serial.println("% rH");

    // uncomment if you want to get temperature in Fahrenheit
    /*temperatureF = 1.8 * bme.readTemperature() + 32;
    Serial.print("Temperature (ºC): ");
    Serial.println(temperatureF);*/

    // set the fields with the values
    ThingSpeak.setField(1, temp.temperature);
    // ThingSpeak.setField(1, temperatureF);
    ThingSpeak.setField(2, humidity.relative_humidity);

    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if (x == 200)
    {
      Serial.println("Channel update successful.");
    }
    else
    {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }
}

void setup() //================================== SETUP ============================
{
  Serial.begin(115200);
  delay(10);
  
  ThingSpeak.begin(client1);  // Initialize ThingSpeak

  // initialize the lcd
  lcd.init();
  lcd.backlight();

  // initialize wifi manager
  WiFiManager wifiManager;

  if (!wifiManager.autoConnect("esp32noCam", "esp32nocam"))
  {
    Serial.println("failed to connect and hit timeout");
    // reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

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

  // initialize aht10
  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");

  pinMode(relayPIN, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(pirMotion, INPUT);
  pinMode(bOut, OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
  client.loop();
  openDoor();
  if (millis() - ttimeLcd >= 10000 & lcdFlag == 1)
  {
    lcdFlag = 0;
    lcd.clear();
  }
}