#include "SingleSegment.h"

SevenSegment::SevenSegment(const int pins[], int dp_pin)
{
    _pins = pins;
    _dp_pin = dp_pin;

    for(uint8_t i=0; i<7; i++)
    {
        pinMode(_pins[i], OUTPUT);
        digitalWrite(_pins[i], LOW);
    }

    pinMode(_dp_pin, OUTPUT);
    digitalWrite(_dp_pin, LOW);
}

SevenSegment::SevenSegment(const int pins[], int dp_pin, bool common_cathode)
{
    _pins = pins;
    _dp_pin = dp_pin;
    _common_cathode = common_cathode;

    for(uint8_t i=0; i<7; i++)
    {
        pinMode(_pins[i], OUTPUT);
        if(common_cathode)
        { digitalWrite(_pins[i], LOW); }
        else
        { digitalWrite(_pins[i], HIGH); }
    }
    pinMode(_dp_pin, OUTPUT);
    digitalWrite(_dp_pin, LOW);
    if(common_cathode)
    {
        digitalWrite(_dp_pin, LOW);
    }
    else
    {
        digitalWrite(_dp_pin, HIGH);
    }
}

uint8_t SevenSegment::get_digit(void)
{
    return _current_digit;
}

void SevenSegment::set_digit(uint8_t digit)
{
    uint8_t pattern;
    if(digit > 0xf)
    {return;}
    _current_digit = digit;

    pattern = _digit_patterns[digit];

    // Set segments (including decimal point if needed)
    for (int i = 0; i < 7; i++) {
        if(_common_cathode)
        {
            digitalWrite(_pins[i], (pattern >> i) & 0x01);
        } else
        {
            digitalWrite(_pins[i], !( (pattern >> i) & 0x01) );
        }
    }
}

void SevenSegment::set_decimal(bool state)
{
    digitalWrite(_dp_pin, state);
}
