#ifndef __DISK_H
#define __DISK_H
#include <stdint.h>
#include <stdbool.h>

typedef struct 
{
  bool (*init)(void);
  void (*write)(uint32_t address, uint8_t* buff, uint32_t size);   /* data */
  void (*read)( uint32_t address, uint8_t* p_out_buff, uint32_t size);
  void (*disk_erase)(void);
}disk_config_t;

/*
   link all the possible flash and check which one is avaiable.
*/
typedef struct disk_config_link_list
{
  disk_config_t disk_config;
  struct disk_config_link_list *next;
}disk_config_link_list_t;

extern disk_config_t *p_disk;

bool disk_init(void);

void disk_register(const disk_config_link_list_t * p_disk_candidate);

#endif