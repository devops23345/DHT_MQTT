// *****************************************************************************
// Program
// *****************************************************************************

#include <Arduino.h>
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ArduinoJson.h>

// local libraries
#include <nodemculed.h> //Controls blinking leds
#include <EasyNodeMCU_Wifi.h>
#include <EasyNodeMCU_MQTT.h>
#include <EasyAsyncWebServer.h>

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
const char* mqtt_sub_topic_3 = "general/local_time";

// JSON
// JSON objects
const int capacity = JSON_OBJECT_SIZE(3);
StaticJsonDocument<capacity> doc;
// Declare a buffer to hold the serialized JSON object
char output_json[128];

int current_hour;
int current_minute;
int current_second;
int current_day;
int current_month;
int current_year;
String time_stamp;
String date_stamp;

// AsyncWebServer
// AsyncWebServer bindings
float &easyAsyncWebServer_Temp = t;//defined in main
float &easyAsyncWebServer_Humidity = h;//defined in main
String &easyAsyncWebServer_TimeStamp = time_stamp;//defined in main
String &easyAsyncWebServer_DateStamp = date_stamp;//defined in main
int easyAsyncWebServer_Port = 80;

// Initialise the WiFi and MQTT Client objects
WiFiClient easyWiFiClient;
// 1883 is the listener port for the Broker
PubSubClient client(easy_MQTT_server, 1883, easyWiFiClient);
#define MSG_BUFFER_SIZE	(100)
char msg[MSG_BUFFER_SIZE];


// *****************************************************************************
// Function Definitions
// *****************************************************************************
void callback_mqtt(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<256> doc;

  // Topic filtering
  if (String(mqtt_sub_topic_3).equals(String(topic))){
    Serial.print("Filtered Message arrived [");
    Serial.print(topic);
    Serial.print("]");
    Serial.println("");
    // JSON message arrived.  Copy payload as it will be overwritten
    DeserializationError err = deserializeJson(doc, payload, length);
    if (err) {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(err.c_str());
    }
    else{
      // Produce a prettified JSON document for serial port
      serializeJsonPretty(doc, Serial);
      //extract the content
      Serial.println("");
      current_hour = doc["current_time"]["hour"];
      current_minute = doc["current_time"]["minute"];
      //current_second = doc["current_time"]["second"];
      current_day = doc["current_date"]["day"];
      current_month = doc["current_date"]["month"];
      current_year = doc["current_date"]["year"];
      time_stamp = String(String(current_hour) + ":" + String(current_minute));
      Serial.print("time_stamp= ");
      Serial.println(time_stamp);
      date_stamp = String(String(current_month) + "/" + String(current_day) +
                          "/" + String(current_year));
    }

  }
  else{ //unflitered messages
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("]");
    Serial.println("");
    for (unsigned int i=0;i < length;i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println("");
  }
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
  if (client.subscribe(mqtt_sub_topic_3)) {
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

// *****************************************************************************
// setup()
// *****************************************************************************
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

  //AsyncWebServer
  setup_easyAsyncWebServer();

  // MQTT
  setup_easy_mqtt();
  client.setCallback(callback_mqtt);
  setup_mqtt_subscriptions();
  client.publish(mqtt_topic_2, "ESP8266-1");
  Serial.print("MQTT Topic: ");
  Serial.print(mqtt_topic_2);
  Serial.println(F(" ESP8266-1 Client Online"));

  //JSON
  doc["sensor"] = "DHT-22";//sensor type

}

// *****************************************************************************
// Main loop()
// *****************************************************************************
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

    //JSON format
    doc["temp_f"] = t;
    doc["humidity"] = h;
    serializeJson(doc, output_json);
    // Produce a prettified JSON document for serial port
    serializeJsonPretty(doc, Serial);

    snprintf(msg, MSG_BUFFER_SIZE, output_json);
    Serial.println("");
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
