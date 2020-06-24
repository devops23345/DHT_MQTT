#ifndef EASYWIFI_H
#define EASYWIFI_H
/*
    easyWiFi -
*/
#include <ESP8266WiFi.h> // Enables the ESP8266 to connect to the local network (via WiFi)
// WiFi
// Make sure to update this for your own WiFi network!
extern const char* easy_Wifi_SSID; //defined in main program
extern const char* easy_Wifi_Password; //defined in main program

// WiFiClient easyWiFiClient;

void setup_easy_wifi(const char*, const char*);
void loop_easy_wifi();

#endif /* EASYWIFI_H */

