#include "eeprom.h"
#include "log.h"
#include "disk.h"
#include "stm32f4xx_hal.h"
#define WRITE 0u
#define READ  1u
#define EEPROM_ADDR 0xa0

#define EEPROM_ADDR_READ 0xa1u
#define EEPROM_ADDR_WRITE 0xa0u

bool eeprom_init(void)
{
  if(HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_ADDR_WRITE, 5,0xFF) != HAL_OK)
  {
    LOG_ERROR("I2C init failed %s", __FUNCTION__);
    HAL_Delay(1000);
    return false;
  }
  else
  {
    LOG_INFO("I2C initialise done!!");
  }
  return true;
}

void eeprom_write(uint32_t address, uint8_t* buff, uint32_t size)
{
   if(HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDR_WRITE, address, I2C_MEMADD_SIZE_8BIT, buff, sizeof(buff), HAL_MAX_DELAY) != HAL_OK)
   {
     LOG_ERROR("EEPROM Write failed!!");
   }
}

void eeprom_read(uint32_t address, uint8_t* p_out_buff, uint32_t size)
{
   if(HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDR_WRITE, address, I2C_MEMADD_SIZE_8BIT, p_out_buff, sizeof(size), HAL_MAX_DELAY) != HAL_OK)
   {
     LOG_ERROR("EEPROM Read failed!!");
   }
}

void eeprom_erase(void)
{
  uint8_t erase_buff[256] = {0};
  for(uint16_t i = 0; i < 256; i++)
  {
    eeprom_write(i*256, erase_buff, sizeof(erase_buff));
    HAL_Delay(5);
  }
}
static const disk_config_link_list_t eeprom_disk = {
    .disk_config = {
      .init = eeprom_init,
      .write = eeprom_write,
      .read = eeprom_read,
      .disk_erase = eeprom_erase
    },
    .next = NULL
  };

void eeprom_driver_register(void)
{
   disk_register(&eeprom_disk);
}


