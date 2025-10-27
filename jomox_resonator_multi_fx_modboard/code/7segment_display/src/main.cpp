#include <Arduino.h>
#include "SingleSegment.h"

#ifdef __ATTINY_MCU__
 #define SEGMENT_A_PIN 5
 #define SEGMENT_B_PIN 4
 #define SEGMENT_C_PIN 2
 #define SEGMENT_D_PIN 1
 #define SEGMENT_E_PIN 0
 #define SEGMENT_F_PIN 5
 #define SEGMENT_G_PIN 7
 #define SEGMENT_DP_PIN 3

 #define BANK_DWN_BTN_PIN 5
 #define BANK_UP_BTN_PIN 6
#else
 #define SEGMENT_A_PIN 13
 #define SEGMENT_B_PIN 12
 #define SEGMENT_C_PIN 18
 #define SEGMENT_D_PIN 17
 #define SEGMENT_E_PIN 16
 #define SEGMENT_F_PIN 14
 #define SEGMENT_G_PIN 15
 #define SEGMENT_DP_PIN 19

 #define BANK_DWN_BTN_PIN 20
 #define BANK_UP_BTN_PIN 21
#endif

#define BTN_DEBOUNCE_MS 300
#ifndef MAX_BANK_NUM
 #define MAX_BANK_NUM 7
#endif

uint32_t prev_millis_btn = 0;
int8_t current_bank = 0;

const int segment_pins[] = {
 SEGMENT_A_PIN,
 SEGMENT_B_PIN,
 SEGMENT_C_PIN,
 SEGMENT_D_PIN,
 SEGMENT_E_PIN,
 SEGMENT_F_PIN,
 SEGMENT_G_PIN,
};

SevenSegment digit(segment_pins, SEGMENT_DP_PIN, false);

void setup()
{
    pinMode(BANK_UP_BTN_PIN, INPUT_PULLUP);
    pinMode(BANK_DWN_BTN_PIN, INPUT_PULLUP);

    // show initial bank number
    digit.set_digit(current_bank);
    digit.set_decimal(true);
}

void loop()
{
    if(millis() - prev_millis_btn >= BTN_DEBOUNCE_MS)
    {
        if(!digitalRead(BANK_UP_BTN_PIN))
        {
            current_bank++;
            if(current_bank > MAX_BANK_NUM) { current_bank = 0; }
            digit.set_digit(current_bank);
            while(!digitalRead(BANK_UP_BTN_PIN)) { delay(1); }
        }
        if(!digitalRead(BANK_DWN_BTN_PIN))
        {
            current_bank--;
            if(current_bank < 0) { current_bank = MAX_BANK_NUM; }
            digit.set_digit(current_bank);
            while(!digitalRead(BANK_DWN_BTN_PIN)) { delay(1); }
        }
        prev_millis_btn = millis();
    }
}
