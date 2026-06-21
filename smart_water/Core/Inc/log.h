#ifndef __LOG_H
#define __LOG_H
#include "ring_buff.h"
#include <stdint.h>
#include <stdbool.h>

#define LOG_BUFF_LENGTH (1024*2)

typedef enum
{
  LOG_MSG_INFO,
  LOG_MSG_WARN,
  LOG_MSG_ERROR,
}log_level_t;

typedef struct
{
  uint8_t rx_buff[LOG_BUFF_LENGTH];
	uint8_t tx_buff[LOG_BUFF_LENGTH];
  log_level_t loglevel;
  ring_buff_t * p_ring_buff;
  bool tx_busy;
  bool rx_lock;
}system_log_t;

extern system_log_t Log;


void log_init(log_level_t loglevel);

void log_write(log_level_t loglevel, char* fmt, ...);

#define LOG_INFO(fmt, ...)  do{log_write(LOG_MSG_INFO, fmt,  ##__VA_ARGS__);}while(0)
#define LOG_WARN(fmt, ...)  do{log_write(LOG_MSG_WARN, fmt,  ##__VA_ARGS__);}while(0)
#define LOG_ERROR(fmt, ...) do{log_write(LOG_MSG_ERROR, fmt,  ##__VA_ARGS__);}while(0)



#endif
