#include "lora.h"
#include "usart.h"
#include "log.h"
lora_config_t lora = {

  .lora_rbuff = {0},
  .rx_flag = false
};

void lora_init(void)
{ 
  if(HAL_UARTEx_ReceiveToIdle_DMA(&huart1, lora.lora_rbuff, sizeof(lora.lora_rbuff)) != HAL_OK)
  {
    LOG_ERROR("Cannot enable the LORA receive function!!");
  }
  else
  {
    LOG_INFO("lora initialisation success!!");
  }
}
