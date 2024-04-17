#include <WiFiNINA.h>
#include <BH1750FVI.h>
#include "secret.h"

WiFiClient client;
BH1750FVI lightSensor(BH1750FVI::k_DevModeContLowRes);

char   HOST_NAME[] = "maker.ifttt.com";
String sunlightDetected = "/trigger/sunlight_detected/with/key/ftZTfUoTBUsCdmZuV01bP";
String sunlightNotDetected = "/trigger/sunlight_not_detected/with/key/ftZTfUoTBUsCdmZuV01bP";
bool isSunlightDetected = false;
bool isSunlightNotDetected = false;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

void setup() {
  // initialize serial communication
  Serial.begin(9600);
  while (!Serial);

  // initialize WiFi connection
if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  // initialize BH1750FVI sensor
  lightSensor.begin();
}

void loop() {
  // read light intensity from sensor
  uint16_t lux = lightSensor.GetLightIntensity();

  // check if sunlight is detected based on light intensity threshold
  if (lux > 100) {
    if (!isSunlightDetected) {
      sendIFTTTNotification(sunlightDetected);
      isSunlightDetected = true;
      isSunlightNotDetected = false;
    }
  } else {
    if (!isSunlightNotDetected) {
      sendIFTTTNotification(sunlightNotDetected);
      isSunlightDetected = false;
      isSunlightNotDetected = true;
    }
  }

  delay(1000); // delay for stability
}

void sendIFTTTNotification(String eventName) {
  if (client.connect(HOST_NAME, 80)) {
    // send HTTP request
    client.print("GET " + eventName + " HTTP/1.1\r\n" +
                 "Host: " + String(HOST_NAME) + "\r\n" +
                 "Connection: close\r\n\r\n");
    Serial.println("IFTTT Notification Sent");
  } else {
    Serial.println("Connection failed");
  }

  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
  }

  client.stop();
}


