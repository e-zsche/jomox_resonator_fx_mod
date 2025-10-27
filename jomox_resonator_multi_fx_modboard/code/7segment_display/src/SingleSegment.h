#ifndef __SINGLE_SEGMENT_H_
#define __SINGLE_SEGMENT_H_

#include <Arduino.h>

class SevenSegment {
public:
    /* Initialize class with array of pins for segments and single pin for decimal point
     * assumes common cathode configuration
     */
    SevenSegment(const int pins[], int dp_pin);
    /* Initialize class with array of pins for segments and single pin for decimal point
     * set common_cathode to false for common anode configuration
     */
    SevenSegment(const int pins[], int dp_pin, bool common_cathode);

    uint8_t get_digit(void);

    void set_digit(uint8_t digit);
    void set_decimal(bool state);

private:
    bool _common_cathode = true;
    const int *_pins = {};
    int _dp_pin;

    uint8_t _current_digit = 0xff;

    // Segment patterns
    const uint8_t _digit_patterns[16] = {
        0b00111111, // 0
        0b00000110, // 1
        0b01011011, // 2
        0b01001111, // 3
        0b01100110, // 4
        0b01101101, // 5
        0b01111101, // 6
        0b00000111, // 7
        0b01111111, // 8
        0b01101111, // 9
        0b01110111, // A
        0b01111100, // B
        0b00111001, // C
        0b01011110, // D
        0b01111001, // E
        0b01110001  // F
    };
};

#endif
