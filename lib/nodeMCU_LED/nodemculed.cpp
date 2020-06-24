#include <Arduino.h>
#include "nodemculed.h"

#define BLNKRTOK 1000 // Blink rate for OK operations = 1sec
#define BLNKRTNOK 200 // Blink rate for Not-OK operations = .2sec

int blinkPeriod;
long lastBlinkMillis;
bool ledState;


void setup_LED(){
  //LED setup
  pinMode(LED_BUILTIN, OUTPUT);//ESP-12E LED
  pinMode(LED_NODEMCU, OUTPUT);//ESP8266 LED
  blinkPeriod = BLNKRTOK;

  // Switch the on-board LED HIGH to start with
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_NODEMCU, LOW);//inverted
}


bool setBlinkState(int state){
    if (state == 0){
        blinkPeriod = BLNKRTOK;
    } else if (state == 1){
        blinkPeriod = BLNKRTNOK;
    }else{
        blinkPeriod = BLNKRTNOK;
        return false;
    }
    return true;
}


// set with LED_STATE_x
bool setNodeMCULEDState(int state){

    if (state == 1){ //turn on
        digitalWrite(LED_NODEMCU, LOW);//NodeMCU LED is Low Active
    }else if(state == 0){
        digitalWrite(LED_NODEMCU, HIGH);//Turn OFF LED
    }else{
        return false;
    }
    return true;
}


void toggle_LED(){
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
}

void loop_LED(){
      unsigned long now = millis();

    if (now - lastBlinkMillis > blinkPeriod)
    {
        toggle_LED();
        lastBlinkMillis = now;
    }

}