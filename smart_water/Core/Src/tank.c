#include "tank.h"
#include "log.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "disk.h"
//#define DEBUG

tank_config_t tank[12];
tank_receive_temp_t tank_info_temp;
static uint8_t tank_enable_number(void);
static uint8_t tank_number_find_by_serial_number(uint8_t * buff);

uint8_t parse_tank_info(uint8_t buff[])
{
  const char* delim = ",";
  uint8_t tank_num = UINT8_MAX;
  char* token = strtok((char*)buff, delim);
  if(strcmp(token, "tank_info") == 0)
  {
    token = strtok(NULL, delim);
    uint8_t tank_num = atoi(token);
    if(tank[tank_num].tank_enable)
    {
      LOG_INFO("The Tank serial number is %s", tank[tank_num].tank_info.serial_number);
      HAL_Delay(10);
      LOG_INFO("The Tank battery level is %d", tank[tank_num].tank_info.battery_level);
      LOG_INFO("The Tank adc level is %d", tank[tank_num].tank_info.adc_level);
      LOG_INFO("The Tank signal level is %d", tank[tank_num].tank_info.signal_level);
    }
    else
    {
      LOG_INFO("This tank does not exist!!");
    }
    return UINT8_MAX;
  }
  else
  {
    if(strcmp(token, "R") == 0)
    {
      tank_info_temp.new_tank = true;
    }
    else if(strcmp(token, "N") == 0)
    {
      tank_info_temp.new_tank = false;
    }

    token = strtok(NULL, delim);
    memcpy((void*)tank_info_temp.tank_info.serial_number, (const void*)token, sizeof(tank_info_temp.tank_info.serial_number));
    tank_info_temp.tank_info.serial_number[8] = '\0';

    token = strtok(NULL, delim);
    tank_info_temp.tank_info.battery_level = atoi(token);

    token = strtok(NULL, delim);
    tank_info_temp.tank_info.adc_level = atoi(token);

    token = strtok(NULL, delim);
    tank_info_temp.tank_info.signal_level = atoi(token);
  }

  if(tank_info_temp.new_tank)
  {
    tank_num = tank_enable_number();
    if(tank_num == UINT8_MAX)
    {
      LOG_ERROR("There is no space for new tank!!");
      return UINT8_MAX;
    }

    tank[tank_num].tank_enable = true;
    LOG_INFO("Receive new Tank, the tank number is %d", tank_num);
  }
  else
  {
    tank_num = tank_number_find_by_serial_number(tank_info_temp.tank_info.serial_number);
    if(tank_num == UINT8_MAX)
    {
      return UINT8_MAX;
    }
  }

  tank[tank_num].tank_info = tank_info_temp.tank_info;
  memset(&tank_info_temp, 0, sizeof(tank_info_temp));
#ifdef DEBUG
    LOG_INFO("The tank serial number is %s", tank[tank_num].tank_info.serial_number);
#endif
  return tank_num;
}

static uint8_t tank_enable_number()
{
  for(uint8_t i = 0; i < TANK_NUM; i++)
  {
    if(tank[i].tank_enable == false)
    {
      return i;
    }
  }
 return  UINT8_MAX;
}

static uint8_t tank_number_find_by_serial_number(uint8_t * buff)
{
  for(uint8_t i = 0; i < 12; i++)
  {
   if(memcmp(tank[i].tank_info.serial_number, buff, sizeof(tank[i].tank_info.serial_number)) == 0)
   {
     return i;
   }
  }
  return UINT8_MAX;
}

void tank_data_restore(void)
{
 for(uint8_t i = 0; i < TANK_NUM; i++)
 {
   p_disk->read(TANKN_ADDRESS(i), (uint8_t*)&tank[i], sizeof(tank[i]));
 }

}

bool is_magic_number_in_disk(void)
{
  uint32_t magic_num = 0;
  p_disk->read(0x00, (uint8_t*)&magic_num, sizeof(uint32_t));
  if(magic_num == MAGIC_NUM)
  {
    return true;
  }
  else
  {
   magic_num = MAGIC_NUM;
   p_disk->write(0x00, (uint8_t*)&magic_num, sizeof(magic_num));
   memset((void*)&tank[0], 0, sizeof(tank));
   for(uint8_t i = 0; i < TANK_NUM; i++)
   {
     p_disk->write(TANKN_ADDRESS(i), (uint8_t*)&tank[i], sizeof(tank_config_t));
   }
  }
  return false;
}
