#include <Arduino.h>
#include <unity.h>
#include "nodemculed.h"

void setup() {
    // // set stuff up here

    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();    // IMPORTANT LINE!
    setup_LED();

}

// void tearDown(void) {
// // clean stuff up here
// }

void test_setNodeMCULEDState_ON(void) {
    setNodeMCULEDState(LED_STATE_ON);

    TEST_ASSERT_EQUAL(LOW, digitalRead(LED_NODEMCU));
}
void test_setNodeMCULEDState_OFF(void) {
    setNodeMCULEDState(LED_STATE_OFF);

    TEST_ASSERT_EQUAL(HIGH, digitalRead(LED_NODEMCU));
}

void test_toggle_LED(void) {
    int currentstate = digitalRead(LED_BUILTIN);
    toggle_LED();
    TEST_ASSERT_EQUAL(!currentstate, digitalRead(LED_BUILTIN));
}

uint8_t i = 0;
uint8_t max_blinks = 5;


void loop(){

    if (i < max_blinks)
    {
        RUN_TEST(test_setNodeMCULEDState_ON);
        delay(500);
        RUN_TEST(test_setNodeMCULEDState_OFF);
        delay(500);
        RUN_TEST(test_toggle_LED);
        i++;
    }
    else if (i == max_blinks) {
      UNITY_END(); // stop unit testing
    }

}