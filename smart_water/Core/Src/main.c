/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "stm32f4xx_hal_rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "log.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "eeprom.h"
#include "flash.h"
#include "disk.h"
#include "lora.h"
#include <string.h>
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "FreeRTOS.h"
#include "tank.h"
#include "semphr.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
TaskHandle_t g_lora_receive_handle = NULL;
TaskHandle_t g_info_parse_handle = NULL;
TaskHandle_t g_info_store_handle = NULL;
TaskHandle_t g_rtos_handle = NULL;
QueueHandle_t g_lora_queue_handle = NULL;

static void vLora_Receive(void *pvParameters);
static void vInfo_Parse(void *pvParameters);
static void vInfo_Store(void * pvParameters);
static void vRTC_Data_Save(void * pvParameters);
static void Save_Tank_Info(void);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  log_init(LOG_MSG_ERROR);
  eeprom_init();
  flash_init();
  lora_init();
  flash_driver_register();
  eeprom_driver_register();
  if(disk_init() == false)
  {
    LOG_ERROR("Disk initialization failed!!");
  }
  else
  {
    LOG_INFO("Disk initialization successful!!");
  }

  if(is_magic_number_in_disk() == true)
  {
   tank_data_restore();
  }

  MX_FREERTOS_Init();
  g_lora_queue_handle = xQueueCreate(5, sizeof(lora.lora_rbuff));
  xTaskCreate(vLora_Receive, "vlorareceive", 128, NULL, 2, &g_lora_receive_handle);
  xTaskCreate(vInfo_Parse, "vInfo_Parse", 256, NULL, 4, &g_info_parse_handle);
  xTaskCreate(vInfo_Store, "vInfo_Store", 256, NULL, 3, &g_info_store_handle);
  xTaskCreate(vRTC_Data_Save, "vRTC_Data_Save", 256, NULL, 3, &g_rtos_handle);
  osKernelStart();


  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1);
  //{
  //  if(lora.rx_flag)
  //  {
  //    lora.rx_flag = false;
  //    LOG_INFO("The receive data is %s", lora.lora_rbuff);
  //    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, lora.lora_rbuff, sizeof(lora.lora_rbuff));
  //  }
  //}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
  if(huart->Instance == USART1)
  {
     if(Log.p_ring_buff->finish_copy_flag == true)
     {
        if(ring_buff_is_empty(Log.p_ring_buff))
        {
           Log.tx_busy = false;
        }
        else
        {
         const uint16_t len = ring_buff_get(Log.p_ring_buff);
         HAL_UART_Transmit_DMA(&huart1, Log.p_ring_buff->send_buff, len);
          Log.tx_busy = true;
        }
  }
  else
  {
    Log.tx_busy =false;
  }
  }
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
   if(huart->Instance == USART1)
   {
     if(size > 0)
     {
       lora.lora_rbuff[size] = '\0';
       lora.rx_flag = true;
     }
   }

}


SemaphoreHandle_t xBinarySemaphore;
/* 
 * @brief: Create lora task..
 */
static void vLora_Receive(void *pvParameters)
{
  while(1)
  {
    if(lora.rx_flag)
    {
      lora.rx_flag = false;
      xQueueSend(g_lora_queue_handle, &lora.lora_rbuff, 100);
      HAL_UARTEx_ReceiveToIdle_DMA(&huart1, lora.lora_rbuff, sizeof(lora.lora_rbuff));
    }
    vTaskDelay(10);
  }
}
static void vInfo_Parse(void *pvParameters)
{
 uint8_t lora_buff[sizeof(lora.lora_rbuff)] = {0};
 xBinarySemaphore = xSemaphoreCreateBinary();
 while(1)
 {
    if(xQueueReceive(g_lora_queue_handle, &lora_buff, portMAX_DELAY) == pdPASS)
    {
     const uint8_t tank_num = parse_tank_info(lora_buff); 
     if(tank_num < TANK_NUM)
     {
       xTaskNotify(g_info_store_handle, tank_num, eSetValueWithOverwrite);
     }
    }

    vTaskDelay(100);
 }
}

static void vInfo_Store(void * pvParameters)
{
  while(1)
  {
    uint8_t tank_num = 0;
    if(xTaskNotifyWait(0x00, 0xFFFFFFFF, (uint32_t*)&tank_num, portMAX_DELAY) == pdPASS)
    {
     p_disk->write(TANKN_ADDRESS(tank_num), (uint8_t*)&tank[tank_num], sizeof(tank[tank_num]));
     LOG_INFO("Tank %d is saved!!", tank_num);
    }
  }
  vTaskDelay(100);

}
static void vRTC_Data_Save(void * pvParameters)
{
  RTC_TimeTypeDef real_time;
  RTC_DateTypeDef real_date;
  while(1)
  {
    if(HAL_RTC_GetDate(&hrtc, &real_date, RTC_FORMAT_BIN) != HAL_OK)
    {
     LOG_ERROR("Get RTC date failed!\n");
     HAL_RTC_Init(&hrtc);
    }
    if(HAL_RTC_GetTime(&hrtc, &real_time, RTC_FORMAT_BIN) != HAL_OK)
    {
     LOG_ERROR("Get RTC time failed!\n");
     HAL_RTC_Init(&hrtc);
    }
    if((real_time.Hours == 23) && (real_time.Seconds == 59))
    {
      Save_Tank_Info(); 
    }
    vTaskDelay(1000);
  }
}

static void Save_Tank_Info(void)
{
  p_disk->write(TANKN_ADDRESS(0), (uint8_t*)&tank[0], sizeof(tank));

}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
