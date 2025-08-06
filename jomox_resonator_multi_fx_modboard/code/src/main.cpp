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
 */
#include <Arduino.h>

// set USI_BUF_SIZE in TinyWireM.h to at least 70
//#define USI_BUF_SIZE 80 //!< bytes in message buffer
#include <TinyWireM.h>

#define DEBOUNCE_TIME 250
#define BANK_UP_PIN 4
#define BANK_DOWN_PIN 3

#define EEPROM_PAGE_SIZE 32
#define FV_1_PROGRAM_SIZE 0x200
#define FV_1_BANK_SIZE 0x1000

#define MAX_BANK_NUM 7

#define WP_PIN 1
#define EEPROM_ADDR 0x50

uint8_t page_buffer_1[EEPROM_PAGE_SIZE];
uint8_t page_buffer_2[EEPROM_PAGE_SIZE];

uint8_t current_bank = 1;

// function prototypes
void eeprom_read_page(uint16_t start_addr, uint8_t *buf);       // store 256 bytes from eeprom in buffer
void eeprom_write_page(uint16_t start_addr, uint8_t *buf);    // write 256 bytes to eeprom from buffer

void fv_1_swap_programs(uint16_t from, uint16_t to);
void fv_1_change_bank(uint8_t bank_num);

uint32_t prev_millis_btn = 0;

void setup() {
    pinMode(BANK_UP_PIN, INPUT_PULLUP);
    pinMode(BANK_DOWN_PIN, INPUT_PULLUP);

    pinMode(WP_PIN, OUTPUT);
    digitalWrite(WP_PIN, HIGH);

    // init buffer
    memset(&page_buffer_1, 0, EEPROM_PAGE_SIZE);
    memset(&page_buffer_1, 0, EEPROM_PAGE_SIZE);

    TinyWireM.begin();

    //eeprom_read_page(0, page_buffer_1);
    //eeprom_write_page(0, page_buffer_1);

    fv_1_swap_programs(0*FV_1_PROGRAM_SIZE, 3*FV_1_PROGRAM_SIZE);
}

void loop() {
    if(millis() - prev_millis_btn >= DEBOUNCE_TIME && !digitalRead(BANK_UP_PIN)) {
        prev_millis_btn = millis();
    }
    if(millis() - prev_millis_btn >= DEBOUNCE_TIME && !digitalRead(BANK_DOWN_PIN)) {
        prev_millis_btn = millis();
    }
}

void eeprom_read_page(uint16_t start_addr, uint8_t *buf) {
    TinyWireM.beginTransmission(EEPROM_ADDR);
    TinyWireM.write(start_addr >> 8);
    TinyWireM.write(start_addr & 0xff);
    TinyWireM.endTransmission();

    TinyWireM.requestFrom(EEPROM_ADDR, EEPROM_PAGE_SIZE);
    for(uint8_t i=0; i<EEPROM_PAGE_SIZE; i++) {
        buf[i] = TinyWireM.read();
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

void fv_1_swap_programs(uint16_t from_addr, uint16_t to_addr) {
    for(uint8_t i=0; i<FV_1_PROGRAM_SIZE/EEPROM_PAGE_SIZE; i++) {
        // buffer page from program 1
        eeprom_read_page(from_addr+i*EEPROM_PAGE_SIZE, page_buffer_1);
        // buffer page from program 2
        eeprom_read_page(to_addr+i*EEPROM_PAGE_SIZE, page_buffer_2);

        // write page from program 1 to program 2
        eeprom_write_page(to_addr+i*EEPROM_PAGE_SIZE, page_buffer_1);
        // write page from program 2 to program 1
        eeprom_write_page(from_addr+i*EEPROM_PAGE_SIZE, page_buffer_2);
    }
}

void fv_1_change_bank(uint8_t bank_num) {
    for(uint8_t i=0; i<FV_1_BANK_SIZE/EEPROM_PAGE_SIZE; i++) {
        // insert pages in FV-1 addressable memory space
        eeprom_read_page(bank_num*FV_1_BANK_SIZE+i*EEPROM_PAGE_SIZE, page_buffer_1);
        eeprom_write_page(bank_num*FV_1_BANK_SIZE+i*EEPROM_PAGE_SIZE, page_buffer_2);
    }
}
