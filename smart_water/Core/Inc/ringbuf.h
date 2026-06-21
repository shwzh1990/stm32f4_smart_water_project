#ifndef __RING_BUF_H
#define __RING_BUF_H

#include <stdbool.h>
#include <stdint.h>
#define RINGBUFF_LENGTH 2048u
#define SEND_BUFF_LENGTH 512u

typedef struct
{
  uint32_t write_index;
	uint32_t read_index;
	uint8_t  ring_buff[RINGBUFF_LENGTH];
  uint8_t  send_buff[SEND_BUFF_LENGTH];
  bool tx_busy;
}ring_buff_config_t;

ring_buff_config_t ring_buff_config;

uint16_t ring_buff_pop(ring_buff_config_t* p_ring_config);
bool ring_buff_put(uint8_t* buff, uint16_t size);
void ring_buff_init(void);


#endif
