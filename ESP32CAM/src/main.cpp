#include <WiFiManager.h>

void setup()
{
  Serial.begin(115200);
  delay(10);

  WiFiManager wifiManager;

  if (!wifiManager.autoConnect("esp32cam", "esp32cam")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:

}