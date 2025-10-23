#include "tiny_i2c.h"

void tiny_init_i2c() {
    // i2c pins as input -> bus is in pullup state
    DDRB &= ~((1<<SDA_PIN) | (1<<SCL_PIN));

    USIDR = 0xFF; // initialise with released state

    USICR = (0<<USISIE) | (0<<USIOIE) | // disable interrupts
            (1<<USIWM1) | (0<<USIWM0) | // two wire mode
            (1<<USICS1) | (0<<USICS0) | // external, positive edge clock source
            (1<<USICLK) | // software clock strobe for 4-bit counter
            (0<<USITC);

    USISR = (1<<USISF)   | (1<<USIOIF)  | // clear all flags
            (1<<USIPF)   | (1<<USIDC)   | // clear all flags
            (0<<USICNT3) | (0<<USICNT2) | // clear counter
            (0<<USICNT1) | (0<<USICNT0);  // clear counter
}
