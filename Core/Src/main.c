/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/******************************************************************************
uart1引脚被DCMIPP复用，禁止使用

******************************************************************************/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32n6xx_hal_def.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define DEBUG 1
#ifdef DEBUG
#include "./HyperRAM/hyperram.h"
#include "./SYS/sys.h"
#endif
#include "./LED/led.h"
#include "./OV5640/ov5640.h"
#include "./RGBLCD/rgblcd.h"
#include "./UART/uart.h"
#include "./NORFlash/norflash.h"
#include "app_x-cube-ai.h"
#include "bsp_init.h"
#include "hw_init.h"
#include "rif_config.h"
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"


UART_HandleTypeDef UartHandle;
#ifdef DEBUG
static HyperRAM_ObjectTypeDef HyperRAMObject = {0};
static NORFlash_ObjectTypeDef NORFlashObject = {0};
#endif
uint16_t img_width;
uint16_t img_height;
uint16_t img_x_offset;
uint16_t img_y_offset;
volatile uint16_t dcmipp_frame_ready = 0;
extern volatile uint16_t Process_OK;
static StaticTask_t main_thread;
static StackType_t main_thread_stack[8192];  /* 32KB 任务栈 */

static void main_freertos(void);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

  MEMSYSCTL->MSCR |= MEMSYSCTL_MSCR_ICACTIVE_Msk;
  MEMSYSCTL->MSCR |= MEMSYSCTL_MSCR_DCACTIVE_Msk;

  SCB_EnableICache();
  SCB_EnableDCache();

  SystemCoreClockUpdate();
  HAL_Init();

#ifdef DEBUG
  sys_clock_config_debug();
#endif

  SystemCoreClockUpdate();
#ifdef DEBUG
  MX_XSPI1_Init();
  HyperRAM_Init(&HyperRAMObject, &hxspi1);
  HyperRAM_EnableMemoryMappedMode(&HyperRAMObject);
  MX_XSPI2_Init();
  NORFlash_Init(&NORFlashObject, &hxspi2, HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_XSPI2));
  NORFlash_EnableMemoryMappedMode(&NORFlashObject);
#endif
  main_freertos();    /* vTaskStartScheduler 不会返回 */
  while (1);          /* 安全兜底 */
}
void app_run(void)
{
  if (Process_OK) {
    Process_OK = 0;
    /* PIPE2 帧就绪 → 数据预处理 → 推理 → 后处理 */
    acquire_and_process_data();
    aiRun();
    post_process();
    // 1. 清空上一帧覆盖层
    rgblcd_overlay_clear();

    // 2. 获取 AI 检测结果
    uint32_t count;
    Detection *hand = get_results(&count);

    // 3. 画到覆盖层
    for (uint32_t i = 0; i < count; i++) {
      // 归一化坐标 → 屏幕像素
      uint16_t cx = hand[i].cx * rgblcddev.width;
      uint16_t cy = hand[i].cy * rgblcddev.height;
      uint16_t bw = hand[i].w * rgblcddev.width;
      uint16_t bh = hand[i].h * rgblcddev.height;

      // 边界框
      uint16_t x1 = cx - bw / 2, y1 = cy - bh / 2;
      uint16_t x2 = cx + bw / 2, y2 = cy + bh / 2;
      rgblcd_draw_rectangle(Layer2, x1, y1, x2, y2, RED);

      // 7 个手部关键点
      for (int k = 0; k < 7; k++) {
        uint16_t kx = hand[i].kps[k][0] * rgblcddev.width;
        uint16_t ky = hand[i].kps[k][1] * rgblcddev.height;
        rgblcd_draw_circle(Layer2, kx, ky, 3, GREEN);
      }
    }

    /* 处理完毕，重新开启 Pipe2 帧中断 */
    __HAL_DCMIPP_ENABLE_IT(&hdcmipp, DCMIPP_IT_PIPE2_FRAME);
  }
  /* USER CODE END WHILE */
}

/*基本初始化*/
static void main_thread_fct(void *arg)
{
	uint32_t preemptPriority;
	uint32_t subPriority;
	IRQn_Type i;

	/* Copy SysTick_IRQn priority set by RTOS and use it as default priorities for IRQs. We are now sure that all irqs
	 * have default priority below or equal to configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY.
	 */
	HAL_NVIC_GetPriority(SysTick_IRQn, HAL_NVIC_GetPriorityGrouping(), &preemptPriority, &subPriority);
	for (i = PVD_PVM_IRQn; i <= LTDC_UP_ERR_IRQn; i++)
    	HAL_NVIC_SetPriority(i, preemptPriority, subPriority);
	STM32CubeAI_Studio_AI_Init();
	hw_init_all();
	rif_config();
	bsp_init_all();
	app_run();
  	vTaskDelete(NULL);
}


#if 0

#endif


static void main_freertos(void)
{
  TaskHandle_t hdl;

  hdl = xTaskCreateStatic(main_thread_fct, "main", 8192, NULL, tskIDLE_PRIORITY + 1,
                          main_thread_stack, &main_thread);
  assert(hdl != NULL);

  vTaskStartScheduler();
  assert(0);

  while (1);
}



/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
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
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
