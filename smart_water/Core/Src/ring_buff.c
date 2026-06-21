/*!
 * @Brief: ring buffer for serial port usage.
 *         this is a buffer is an cache from TX to peripheral.
 *
 *         When buffer is full, then new data will overwrite the old data.
 *         Please be careful.
 **/
#include "ring_buff.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))
static void buff_memset(uint8_t* p, uint8_t value, uint32_t size);
static uint16_t ring_buff_get_store_data_space(ring_buff_t * p_ring_buff);

ring_buff_t ring_buff_register(uint8_t* buff, uint32_t buffer_size)
{
  ring_buff_t ring_buf;
  buff_memset((uint8_t*)&ring_buf, 0, sizeof(ring_buf));
  ring_buf.buff = buff;
  ring_buf.buff_size = buffer_size;
  return ring_buf;
}


/*!
  @breif: Here, after the DMA tx complete interrupt is triggered, 
          The code will check if the ring_buff has more info, and send all of the rest content if          so. Then the code will reset the read_index and write_index to the start of the buff.

*/
bool ring_buff_is_empty(ring_buff_t* ring_buff)
{
   return ring_buff->read_index == ring_buff->write_index;
}

bool ring_buff_is_full(ring_buff_t* p_ring_buff)
{
  return ((p_ring_buff->write_index + 1) % p_ring_buff->buff_size) == p_ring_buff->read_index;
}


void ring_buff_set_empty(ring_buff_t* p_ring_buff)
{
   p_ring_buff->read_index = 0;
   p_ring_buff->write_index = 0;
 //  buff_memset(ring_buff->send_buff, 0, sizeof(ring_buff->send_buff));
}

/*!
 * @brief: return how many bytes is written 
 *         so the one slot is used for checking empty and full.
 * */
uint16_t ring_buff_write(ring_buff_t* ring_buff, uint8_t* data, uint32_t size){

    uint16_t buffer_write_space = 0;
    ring_buff->finish_copy_flag = false;
    if(ring_buff->write_index > ring_buff->read_index)
    {
       
       buffer_write_space = ring_buff->buff_size - (ring_buff->write_index - ring_buff->read_index) - 1;
    }
    else if(ring_buff->write_index == ring_buff->read_index)
    {
       ring_buff->write_index = 0;
       ring_buff->read_index = 0; 
       buffer_write_space = ring_buff->buff_size - 1;
    }
    else if((ring_buff->read_index > ring_buff->write_index) && (!ring_buff_is_full(ring_buff)))
    {
        buffer_write_space = ring_buff->read_index - ring_buff->write_index;
    }

    const uint16_t writable_size = MIN(size, buffer_write_space);

    for(uint16_t i = 0; i < writable_size; i++)
    {
       if(((ring_buff->write_index + 1) % ring_buff->buff_size) != ring_buff->read_index)
       {
          ring_buff->buff[(ring_buff->write_index++ % ring_buff->buff_size)] = data[i];
       }
    }
    ring_buff->finish_copy_flag = true;
    return writable_size;
}

/*!
 * @brief: copy all log string from ringbuff to ring_buff->send_buff
 *         return how many data should be fetch.
 * */
uint16_t ring_buff_get(ring_buff_t* p_ring_buff)
{
  uint16_t len = 0;
  if(p_ring_buff->write_index > p_ring_buff->read_index)
  {
   len = p_ring_buff->write_index - p_ring_buff->read_index;
   
  }
  else if(p_ring_buff->read_index > p_ring_buff->write_index)
  {
    len = p_ring_buff->buff_size - p_ring_buff->read_index + p_ring_buff->write_index;
  }

  const uint16_t buffer_takes_data_length = MIN(len, sizeof(p_ring_buff->send_buff));
  for(uint16_t i = 0; i < buffer_takes_data_length; i++)
   {
     p_ring_buff->send_buff[i] = p_ring_buff->buff[p_ring_buff->read_index % p_ring_buff->buff_size];
     p_ring_buff->read_index++;
     p_ring_buff->read_index %= p_ring_buff->buff_size;
   }
   return buffer_takes_data_length; 
}

static uint16_t ring_buff_get_store_data_space(ring_buff_t * p_ring_buff)
{
  if(p_ring_buff->write_index >= p_ring_buff->read_index)
  {
    return p_ring_buff->write_index - p_ring_buff->read_index;
  }
  return p_ring_buff->buff_size - p_ring_buff->read_index + p_ring_buff->write_index;
}

static void buff_memset(uint8_t* buff, uint8_t value, uint32_t size)
{
   for(uint32_t i = 0; i < size; i++)
   {
     if(buff != NULL)
     {
        *buff = value;
     }
     buff++;
   }

}

