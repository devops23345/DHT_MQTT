#include "EasyNodeMCU_Wifi.h"
#include <nodemculed.h>
#include <ESP8266WiFi.h> // Enables the ESP8266 to connect to the local network (via WiFi)


void setup_easy_wifi(const char* ssid, const char* password){

  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    toggle_LED();
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void loop_easy_wifi(){
  // WiFi
  // Check status.  Reconnect if needed.
  // This is a blocking call
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnect Wifi::");
    setup_easy_wifi(easy_Wifi_SSID, easy_Wifi_Password);
  }
}
