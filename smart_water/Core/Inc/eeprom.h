#ifndef EEPROM_H
#define EEPROM_H
#include "i2c.h"
#include <stdint.h>

void eeprom_init(void);
void eeprom_write(uint16_t address, uint8_t* buff, uint16_t size);
void eeprom_read(uint16_t address, uint8_t* buff, uint16_t size);

#endif
