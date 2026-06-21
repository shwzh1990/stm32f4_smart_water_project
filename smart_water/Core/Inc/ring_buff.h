#ifndef __RING_BUFF_H
#define __RING_BUFF_H
#include <stdint.h>
#include <stdbool.h>
typedef struct
{
  uint8_t* buff;
  uint8_t send_buff[512];     //so for now the debug log string should be less then 512.
  uint16_t read_index;
  uint16_t write_index;
  uint16_t buff_size;         //record the ringbuff size.
  bool finish_copy_flag;      //Set true if data has been copied to the ring buffer.
}ring_buff_t;



ring_buff_t ring_buff_register(uint8_t* buff, uint32_t buffer_size);

bool ring_buff_is_empty(ring_buff_t* ring_buff);

void ring_buff_set_empty(ring_buff_t* ring_buff);

uint16_t ring_buff_write(ring_buff_t* ring_buff, uint8_t* data, uint32_t size);

uint16_t ring_buff_get(ring_buff_t* ring_buff);
#endif
