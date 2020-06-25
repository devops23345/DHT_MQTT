/*********
 *********/

// Import required libraries
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "EasyAsyncWebServer.h"

// Create AsyncWebServer object on port 80
AsyncWebServer server(serverport);

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(easyAsyncWebServer_Temp);
  }
  else if(var == "HUMIDITY"){
    return String(easyAsyncWebServer_Humidity);
  }
  return String();
}

void setup_easyAsyncWebServer(){

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(easyAsyncWebServer_Temp).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(easyAsyncWebServer_Humidity).c_str());
  });

  // Start server
  server.begin();
}

void loop(){

}
