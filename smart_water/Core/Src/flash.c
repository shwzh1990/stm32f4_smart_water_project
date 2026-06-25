#include "flash.h"
#include "log.h"
#include "disk.h"
#include <string.h>
#define ID_CMD  0xABu
#define FLASH_SECTOR_SIZE 4096u


static bool is_flash_busy(void);
static void flash_cs_on(void)
{
  HAL_GPIO_WritePin(F_CS_GPIO_Port, F_CS_Pin, GPIO_PIN_RESET);
}

static void flash_cs_off(void)
{
  HAL_GPIO_WritePin(F_CS_GPIO_Port, F_CS_Pin, GPIO_PIN_SET);
}


static void flash_write_enable(void)
{
  const uint8_t write_enable_cmd = 0x06;
  flash_cs_on();
  HAL_SPI_Transmit(&hspi1, &write_enable_cmd, 1, 0xFF);
  flash_cs_off();
}

static void flash_write_disable(void)
{
  const uint8_t write_disable_cmd = 0x04;
  flash_cs_on();
  HAL_SPI_Transmit(&hspi1, &write_disable_cmd, 1, 0xFF);
  flash_cs_off();
}

static void flash_erase_sector(uint32_t address)
{
  const uint8_t sector_erase_cmd = 0x20;
  flash_write_enable();
  flash_cs_on();
  uint8_t cmd_buff[4] = {sector_erase_cmd, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};
  HAL_SPI_Transmit(&hspi1, cmd_buff, sizeof(cmd_buff), 0xFF);
  flash_cs_off();
  while(is_flash_busy())
  {
    HAL_Delay(5); // Wait for the flash to be ready
  }
}
static uint16_t flash_read_registers_1(void)
{
  const uint8_t read_status_cmd = 0x05;
  uint16_t status = 0;
  flash_cs_on();
  HAL_SPI_Transmit(&hspi1, &read_status_cmd, 1, 0xFF);
  HAL_SPI_Receive(&hspi1, (uint8_t*)&status, sizeof(status), 0xFF);
  flash_cs_off();
  return status;
}

static bool is_flash_busy(void)
{
  const uint16_t busy_mask = 0x01; // Assuming the busy bit is the least significant bit
  return (flash_read_registers_1() & busy_mask) != 0;
}

void chip_erase(void)
{
  const uint8_t chip_erase_cmd = 0xC7;
  flash_write_enable();
  flash_cs_on();
  HAL_SPI_Transmit(&hspi1, &chip_erase_cmd, 1, 0xFF);
  flash_cs_off();
}



static void flash_raw_write(uint32_t address, uint8_t* buff, uint16_t size)
{
  const uint8_t page_program_cmd = 0x02;
  flash_write_enable();
  flash_cs_on();
  uint8_t cmd_buff[4] = {page_program_cmd, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};
  HAL_SPI_Transmit(&hspi1, cmd_buff, sizeof(cmd_buff), 0xFF);
  HAL_SPI_Transmit(&hspi1, buff, size, 0xFF);
  flash_cs_off();
}

bool flash_init(void)
{
  const uint8_t device_id_cmd = ID_CMD;
  const uint8_t dummy_data[3] = {0xFF};
  const uint8_t expected_device_id = 0x17; //Example expected device ID, replace with actual value for your flash chip
  uint8_t device_id = 0;
  flash_cs_on();
  HAL_SPI_Transmit(&hspi1, &device_id_cmd, 1, 0xFF);
  HAL_SPI_Transmit(&hspi1, dummy_data, sizeof(dummy_data), 0xFF);

  HAL_SPI_Receive(&hspi1, &device_id, sizeof(device_id), 0xFF);
 
  flash_cs_off();
  return (device_id == expected_device_id); 
}

uint16_t flash_get_device_id(void)
{
  flash_cs_on();
  const uint8_t device_id_cmd = 0x90;
  const uint8_t dummy_data[3] = {0xFF, 0xFF, 0x00};
  uint16_t manufacture_id = 0;
  HAL_SPI_Transmit(&hspi1, &device_id_cmd, 1, 0xFF);
  HAL_SPI_Transmit(&hspi1, dummy_data, sizeof(dummy_data), 0xFF);
  HAL_SPI_Receive(&hspi1, (uint8_t*)&manufacture_id, sizeof(manufacture_id), 0xFF);
  flash_cs_off();
  return manufacture_id;
}

void flash_write_page(uint32_t address, uint8_t* buff, uint16_t size)
{
  const uint16_t page_size = 256;
  uint16_t bytes_written = 0;

  while (bytes_written < size)
  {
    uint16_t bytes_to_write = page_size - (address % page_size);
    if (bytes_to_write > (size - bytes_written))
    {
      bytes_to_write = size - bytes_written;
    }

    flash_raw_write(address, buff + bytes_written, bytes_to_write);
    while(is_flash_busy())
    {
      HAL_Delay(5); // Wait for the flash to be ready
    }
    flash_write_disable();
    address += bytes_to_write;
    bytes_written += bytes_to_write;
  }
}

uint8_t temp_read_buff[FLASH_SECTOR_SIZE] = {0}; // Buffer to hold data for writing
void flash_write(uint32_t address, uint8_t* buff, uint32_t size)
{
   uint32_t header_address = address & 0xFFFFF000; // Align to 4KB boundary
   uint32_t offset = address - header_address;
   uint32_t write_size_available_in_sector = 4096 - offset; // Calculate how much space is available in the sector from the offset
   do
   {
     flash_read(header_address, temp_read_buff, 4096); // Read the entire sector into the buffer
     flash_erase_sector(header_address);
     if(size > write_size_available_in_sector)
     {
       memcpy(temp_read_buff + offset, buff, write_size_available_in_sector);
       flash_write_page(header_address, temp_read_buff, sizeof(temp_read_buff));
       size -= write_size_available_in_sector;
       write_size_available_in_sector = FLASH_SECTOR_SIZE;
       header_address += FLASH_SECTOR_SIZE;
       offset = 0;
     }
     else
     {
       memcpy(temp_read_buff + offset, buff, size);
       flash_write_page(header_address, temp_read_buff, sizeof(temp_read_buff));
     }
     

   }while(size > write_size_available_in_sector);
      

}

void flash_read(uint32_t address, uint8_t* rbuff, uint32_t size)
{
  const uint8_t read_data_cmd = 0x03;
  flash_cs_on();
  uint8_t cmd_buff[4] = {read_data_cmd, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};
  HAL_SPI_Transmit(&hspi1, cmd_buff, sizeof(cmd_buff), 0xFF);
  HAL_SPI_Receive(&hspi1, rbuff, size, 0xFF);
  flash_cs_off();
}
static const disk_config_link_list_t flash_disk = {
    .disk_config = {
      .init = flash_init,
      .write = flash_write,
      .read = flash_read,
      .disk_erase = chip_erase
    },
    .next = NULL
  };

void flash_driver_register(void)
{
   disk_register(&flash_disk);
}
