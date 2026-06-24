#ifndef FLASH_H
#define FLASH_H
#include "spi.h"
#include <stdint.h>
#include "gpio.h"
#include <stdbool.h>
bool flash_init(void);

uint16_t flash_get_device_id(void);

void flash_write(uint32_t address, uint8_t* buff, uint16_t size);

void flash_read(uint32_t address, uint8_t* rbuff, uint16_t size);

#endif
