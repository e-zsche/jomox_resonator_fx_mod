/* program for extending the loadable FV-1 FX program memory. This project is inspired by the µDervish module.
 *
 * idea:
 *   conventionally the FV-1 can access 8 program slots, each containing 512 Byte over an
 *   I2C bus. The datasheet lists 24LC32 for the program memory.
 *   To get around the limitation of only being able to access these 8 program slots a
 *   microcontroller and larger eeprom of the same variety is used. The FV-1 will still be able to
 *   access the first 8 programs, but will not see any data past the last address (0x7FF). When
 *   you want to select a different bank the microcontroller has to read the bank from eeprom memory
 *   and store it in the data range accessible by the FV-1.
 *
 *   ATTiny85 fuse settings:
 *   hfuse: 0xD7
 *   lfuse: 0xE2
 *   efuse: 0xFF
 *   lock:  0xFF
 *
 *   avrdude fuse write command:
 *     $ avrdude -c stk500v2 -p attiny85 -P /dev/ttyUSB0 -U hfuse:w:0xd7:h -U lfuse:w:0xe2:h -U efuse:w:0xff:h -U lock:w:0xff:h
 */
#include <Arduino.h>

// set USI_BUF_SIZE in TinyWireM.h to at least 70
//#define USI_BUF_SIZE 80 //!< bytes in message buffer
#include <TinyWireM.h>

#define DEBOUNCE_TIME 1000
uint32_t prev_millis_btn = 0;

#define BANK_UP_PIN 4
#define BANK_DOWN_PIN 3

#define EEPROM_PAGE_SIZE 0x40   // 64
#define FV_1_PROGRAM_SIZE 0x200 // 512
#define FV_1_BANK_SIZE 0x1000   // 4096

#define MAX_BANK_NUM 6

#define WP_PIN 1
#define EEPROM_ADDR 0x50
uint8_t page_buffer[EEPROM_PAGE_SIZE];
uint8_t current_bank = 1;
bool bank_changed = false;

// function prototypes
void eeprom_read_page(uint16_t start_addr, uint8_t *buf);     // store page from eeprom in buffer
void eeprom_write_page(uint16_t start_addr, uint8_t *buf);    // write page to eeprom from buffer
void fv_1_change_bank(uint8_t bank_num, uint8_t *buf);
void suspend_i2c_bus();
void activate_i2c_bus();

void setup() {
    pinMode(BANK_UP_PIN, INPUT_PULLUP);
    pinMode(BANK_DOWN_PIN, INPUT_PULLUP);

    //pinMode(WP_PIN, OUTPUT);
    pinMode(WP_PIN, INPUT); // pin is shorted to GND inside the resonator
    digitalWrite(WP_PIN, HIGH);

    // init buffer
    memset(&page_buffer, 0, EEPROM_PAGE_SIZE);

    TinyWireM.begin();
    suspend_i2c_bus();
}

void loop() {
    // bank up
    if(millis() - prev_millis_btn >= DEBOUNCE_TIME && !digitalRead(BANK_UP_PIN)) {
        current_bank++;
        if(current_bank > MAX_BANK_NUM) {
            current_bank = 1;
        }
        bank_changed = true;
        prev_millis_btn = millis();
    }

    // bank down
    if(millis() - prev_millis_btn >= DEBOUNCE_TIME && !digitalRead(BANK_DOWN_PIN)) {
        current_bank--;
        if(current_bank < 1) {
            current_bank = MAX_BANK_NUM;
        }
        bank_changed = true;
        prev_millis_btn = millis();
    }

    if(bank_changed) {
        activate_i2c_bus();
        fv_1_change_bank(current_bank, page_buffer);
        suspend_i2c_bus();
        bank_changed = false;
    }
}

void eeprom_read_page(uint16_t start_addr, uint8_t *buf) {
    TinyWireM.beginTransmission(EEPROM_ADDR);
    TinyWireM.write(start_addr >> 8);
    TinyWireM.write(start_addr & 0xff);
    TinyWireM.endTransmission();

    TinyWireM.requestFrom(EEPROM_ADDR, EEPROM_PAGE_SIZE);
    while(TinyWireM.available() < 63) {;;} // wait for page
    for(uint8_t i=0; i<EEPROM_PAGE_SIZE; i++) {
        buf[i] = TinyWireM.read();
    }
    // flush buffer
    while(TinyWireM.available()) {
        TinyWireM.read();
    }
}

void eeprom_write_page(uint16_t start_addr, uint8_t *buf) {
    digitalWrite(WP_PIN, LOW);
    TinyWireM.beginTransmission(EEPROM_ADDR);
    TinyWireM.write(start_addr >> 8);
    TinyWireM.write(start_addr & 0xff);
    for(uint8_t i=0; i<EEPROM_PAGE_SIZE; i++) {
        TinyWireM.write(buf[i]);
    }
    TinyWireM.endTransmission();
    digitalWrite(WP_PIN, HIGH);
    delay(5);
}

void fv_1_change_bank(uint8_t bank_num, uint8_t *buf) {
    for(uint8_t i=0; i<FV_1_BANK_SIZE/EEPROM_PAGE_SIZE; i++) {
        // insert pages in FV-1 addressable memory space
        eeprom_read_page( bank_num*FV_1_BANK_SIZE + i*EEPROM_PAGE_SIZE, buf);
        eeprom_write_page(i*EEPROM_PAGE_SIZE, buf);
    }
}

void suspend_i2c_bus() {
    // set SCL and SDA to tri-state
    DDRB &= ~(1<<DDB0 | 1<<DDB2);
}

void activate_i2c_bus() {
    // reset SCL and SDA pinMode
    DDRB |= 1<<DDB0 | 1<<DDB2;
}
