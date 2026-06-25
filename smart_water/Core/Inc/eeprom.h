#ifndef EEPROM_H
#define EEPROM_H
#include "i2c.h"
#include <stdint.h>
#include <stdbool.h>

bool eeprom_init(void);
void eeprom_write(uint32_t address, uint8_t* buff, uint32_t size);
void eeprom_read(uint32_t address, uint8_t* buff, uint32_t size);
void eeprom_erase(void);
void eeprom_driver_register(void);
void flash_write_page(uint32_t address, uint8_t* buff, uint16_t size);
#endif
