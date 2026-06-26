#ifndef __LORA_H
#define __LORA_H
#include <stdint.h>
#include <stdbool.h>
#define RECEIVE_LORA_DATA_AMOUNT_MAX 200u

typedef struct
{
  uint8_t lora_rbuff[RECEIVE_LORA_DATA_AMOUNT_MAX];
  bool rx_flag;
}lora_config_t;

extern lora_config_t lora;

void lora_init(void);

#endif
