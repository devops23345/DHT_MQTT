#include <Arduino.h>
/*
 * ESP8266 (Adafruit HUZZAH) Mosquitto MQTT Publish Example
 * Thomas Varnish (https://github.com/tvarnish), (https://www.instructables.com/member/Tango172)
 * Made as part of my MQTT Instructable - "How to use MQTT with the Raspberry Pi and ESP8266"
 */
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <nodemculed.h> //Controls blinking leds
#include <EasyNodeMCU_Wifi.h>
#include <EasyNodeMCU_MQTT.h>

// WiFi
// Make sure to update this for your own WiFi network!
const char* easy_Wifi_Password = "23345chicoryroad";
const char* easy_Wifi_SSID = "Doppler2";

// DHT
//type of sensor in use:
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
#define DHTPIN     D1     // Digital pin GPIO5 connected to the DHT sensor

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 10000;
DHT dht(DHTPIN, DHTTYPE);

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* easy_MQTT_server = "192.168.1.32";
const char* easy_MQTT_username = "devops23345";
const char* easy_MQTT_password = "chicory";

// The client id identifies the ESP8266 device.
String clientID = "ESP8266_1";

// MQTT Topics - Publish
const char* mqtt_topic = "sensors/temp_humidity/location/office";
const char* mqtt_topic_2 = "MQTT_publishers/device_id";

// MQTT Topics - Subscribe
const char* mqtt_sub_topic_1 = "general/#";
const char* mqtt_sub_topic_2 = "MQTT_publishers/#";

// MQTT JSON objects

// Initialise the WiFi and MQTT Client objects
WiFiClient easyWiFiClient;
PubSubClient client(easy_MQTT_server, 1883, easyWiFiClient); // 1883 is the listener port for the Broker

#define MSG_BUFFER_SIZE	(100)
char msg[MSG_BUFFER_SIZE];

void callback_mqtt(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // handle message arrived
}

bool setup_mqtt_subscriptions(){
 //Handle subscriptions
  bool result = false;

  if (client.subscribe(mqtt_sub_topic_2)) {
      Serial.print("MQTT Topic Subscribed: ");
      Serial.print(mqtt_sub_topic_2);
      Serial.println("");
      result = true;
  }
  else
  {
      Serial.println("Not Subscribed");
      result = false;
  }

  if (client.subscribe(mqtt_sub_topic_1)) {
      Serial.print("MQTT Topic Subscribed: ");
      Serial.print(mqtt_sub_topic_1);
      Serial.println("");
      result = true;
  }
  else
  {
      Serial.println("Not Subscribed");
      result = false;
  }
  return result;
}


void setup() {
  // Start serial comm first as other setups will print to the serial port
  // Begin Serial on 115200
  // Remember to choose the correct Baudrate on the Serial monitor!
  // This is just for debugging purposes
  Serial.begin(115200);
  delay(2);

  setup_LED();
  setBlinkState(BLINK_STATE_OK);

  //prep the sensor
  dht.begin();

  //WiFi
  setup_easy_wifi(easy_Wifi_SSID, easy_Wifi_Password);

  // MQTT
  setup_easy_mqtt();
  client.setCallback(callback_mqtt);
  setup_mqtt_subscriptions();
  client.publish(mqtt_topic_2, "ESP8266-1");
  Serial.print("MQTT Topic: ");
  Serial.print(mqtt_topic_2);
  Serial.println(F(" ESP8266-1 Client Online"));

}


void loop() {
 //now is used to measure time periods for loops
  unsigned long now = millis();

  //LED
  loop_LED();

  //WiFi
  loop_easy_wifi();//will handle reconnects

  //MQTT
  loop_easy_mqtt();//will handle reconnects
  client.loop();

  //int cState = client.state();

  //int cConnect = client.connected();
/*   Serial.print(cConnect);
  Serial.println("");
  Serial.print("Client State=");
  Serial.print(cState);
  Serial.println(""); */

  // DHT
  // read temp & humidity
  if (now - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = now;

    // Read temperature as Celsius (the default)
    //float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float newT = dht.readTemperature(true);
    // Read Humidity
    float newH = dht.readHumidity();
    // if read failed, don't change t & h value

    if (isnan(newT) || isnan(newH)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      setBlinkState(BLINK_STATE_NOK);
    } else {
      t = newT;
      Serial.println(t);
      h = newH;
      Serial.println(h);
      setBlinkState(BLINK_STATE_OK);
    }

    // MQTT Data prep
    client.loop();

    snprintf(msg, MSG_BUFFER_SIZE, "Temp =  %3.1f Humidity = %3.1f", t, h);

    Serial.print("Publish message: ");
    Serial.print("MQTT Topic: ");
    Serial.println(mqtt_topic);
    Serial.println(msg);

    // Publish the data to the broker
    if (client.publish(mqtt_topic, msg)) {
        Serial.println("Data Published");
    }
      // Again, client.publish will return a boolean value depending on whether it succeded or not.
      // If the message failed to send, we will try again, as the connection may have broken.
    else {
        Serial.println(F("Message failed to send."));
    }

      // Client connected.  Run loop to check for incoming
    client.loop();
  }
}
