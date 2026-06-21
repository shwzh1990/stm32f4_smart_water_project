#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#define LOG_SUFFIX_LEN  (6u)
#include "stm32f4xx_hal.h"
#include "usart.h"
#include "log.h"
#include "ring_buff.h"

static const char* log_suffix[] = {"INFO: ", "WARN: ", "ERROR:"};

static ring_buff_t log_ring_buff;

system_log_t Log;

void log_init(log_level_t loglevel)
{
  memset((void*)&Log, 0, sizeof(Log));

  Log.loglevel = loglevel;

  log_ring_buff = ring_buff_register(Log.tx_buff, sizeof(Log.tx_buff));
  Log.p_ring_buff = &log_ring_buff;
  Log.tx_busy = false;
}

void log_write(log_level_t loglevel, char* fmt, ...)
{

  char temp_buff[512] = {'\0'};
  if(Log.loglevel >= loglevel)
  {
    va_list args;
    va_start(args, fmt);
    sprintf((char*)temp_buff, "%s",log_suffix[Log.loglevel]);
    const uint16_t n = vsprintf((char*)temp_buff+LOG_SUFFIX_LEN, fmt, args);
    temp_buff[LOG_SUFFIX_LEN + n] = '\n';
    va_end(args);

    (void)ring_buff_write(&log_ring_buff, (uint8_t*)temp_buff, strlen((const char*)temp_buff) + 1);
    if(Log.tx_busy == false)
    {
      const uint16_t len = ring_buff_get(&log_ring_buff);
      HAL_UART_Transmit_DMA(&huart1, log_ring_buff.send_buff, len);
      Log.tx_busy = true;
    }
  }

}
