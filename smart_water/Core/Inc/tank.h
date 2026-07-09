#ifndef __TANK_H
#define __TANK_H

#include <stdint.h>
#include <stdbool.h>
typedef struct
{
  uint8_t serial_number[9];
  uint8_t battery_level;
  uint8_t adc_level;
  uint8_t signal_level;
}tank_receive_from_lora_t;

typedef struct
{
  tank_receive_from_lora_t tank_info;
  bool tank_enable;
  uint8_t water_level_percentage;
  float tank_height;
  uint8_t density;
  uint8_t water_level_percentage_week[7];
  uint8_t water_level_percentage_month[30];
}tank_config_t;

typedef struct 
{
  tank_receive_from_lora_t tank_info;
  bool new_tank;
}tank_receive_temp_t;

#define MAGIC_NUM 0xABCDEFu
#define TANK1_ADDRESS 0x100
#define TANK_NUM 12u
#define TANKN_ADDRESS(tank_num) (TANK1_ADDRESS + sizeof(tank_config_t) * tank_num)
extern tank_config_t tank[12];


uint8_t parse_tank_info(uint8_t* buff);
void tank_data_restore(void);
bool is_magic_number_in_disk(void);

#endif
