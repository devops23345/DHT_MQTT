#ifndef EASY_NODEMCU_LED_H
#define EASY_NODEMCU_LED_H
/*
    easyNodeMCU_LED -
*/


//LED
#define LED_NODEMCU D0 //NodeMCU LED
#define BLINK_STATE_OK    0 // Set to signal everting ok (~1sec Blink)
#define BLINK_STATE_NOK   1 // Set to signal error (~0.2sec Blink)
#define LED_STATE_ON      1 // Set to turn on an LED
#define LED_STATE_OFF     0 // Set to turn off an LED


void setup_LED();
void toggle_LED();
bool setBlinkState(int state); //set with BLINK_STATE_x
bool setNodeMCULEDState(int); //set with LED_STATE_x
void loop_LED(); //must be called in main loop

#endif /* EASY_NODEMCU_LED_H */
