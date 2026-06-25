#include "disk.h"
#include "flash.h"
#include "eeprom.h"
#include "log.h"
#include <string.h>
static disk_config_link_list_t disk_link_list = {0};

disk_config_t * p_disk;

bool disk_init(void)
{
  disk_config_link_list_t *p_current = &disk_link_list;
  do{
     if(p_current == NULL)
     {
        break;
     }
    if((p_current->disk_config.init != NULL) && (p_current->disk_config.init() == true))
     {
        /*
        uint8_t temp_buff[20];
        p_current->disk_config.write(0x0, (uint8_t*)"Disk test", sizeof("Disk test"));
        p_current->disk_config.read(0x0, (uint8_t*)temp_buff, sizeof(temp_buff));
        LOG_INFO("Disk test read: %s", temp_buff); 
        */
        p_disk = &p_current->disk_config; 
      return true;

     }

    p_current = p_current->next;
    }while(p_current != NULL);
    return false;
}

void disk_register(const disk_config_link_list_t * p_disk_candidate)
{
    disk_config_link_list_t *p_current = &disk_link_list;
    while(p_current->next != NULL)
    {
      p_current = p_current->next;
    }
    p_current->next = p_disk_candidate;
}