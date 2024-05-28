#include <WiFiManager.h>



//===========PINS==================
#define relayPIN 25
#define buttonPin 35
#define pirMotion 32
#define bOut 12
#define rOut 14

void setup()
{
  Serial.begin(115200);
  delay(10);

  WiFiManager wifiManager;

  if (!wifiManager.autoConnect("esp32noCam", "esp32nocam")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
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

void loop() {
  // put your main code here, to run repeatedly:

}