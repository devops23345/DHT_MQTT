#ifndef EASYNODEMCU_MQTT_H
#define EASYNODEMCU_MQTT_H
/*
    EASYNODEMCU_MQTT_H -
*/
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker

// Make sure to update this for your broker!
extern const char* easy_MQTT_server;//required
extern const char* easy_MQTT_username;//optional
extern const char* easy_MQTT_password;//optional

// Make sure to update this for your specific clientID!
extern String clientID;

// Make sure to instantiate this in your main loop for your specific broker!
extern PubSubClient client;

// WiFiClient easyWiFiClient;

void setup_easy_mqtt();
void loop_easy_mqtt();

#endif /* EASYNODEMCU_MQTT_H */

