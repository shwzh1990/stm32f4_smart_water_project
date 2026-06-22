#include "eeprom.h"
#include "log.h"
#define WRITE 0u
#define READ  1u
#define EEPROM_ADDR 0x60

#define EEPROM_ADDR_READ 0x61u
#define EEPROM_ADDR_WRITE 0x60u

void eeprom_init(void)
{
  if(HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_ADDR_READ, 5,0xFF) != HAL_OK)
  {
    LOG_ERROR("I2C init failed %s", __FUNCTION__);
  }
}

void eeprom_write(uint16_t address, uint8_t* buff, uint16_t size)
{
   if(HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDR_WRITE, address, I2C_MEMADD_SIZE_8BIT, buff, sizeof(buff), HAL_MAX_DELAY) != HAL_OK)
   {
     LOG_ERROR("EEPROM Write failed!!");
   }
}


