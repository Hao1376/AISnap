/**
 ******************************************************************************
 * @file           : hw_init.h
 * @brief          : Hardware peripheral initialization
 ******************************************************************************
 * @attention
 *
 * All MX_*_Init functions extracted from main.c to keep main() thin.
 * Hardware handles declared here so they are accessible to BSP and ISR code.
 *
 * FreeRTOS note:
 *   When migrating to FreeRTOS, hardware initialization should complete
 *   before the scheduler starts (vTaskStartScheduler). The handles below
 *   may be shared across tasks — protect with mutex/semaphore as needed.
 ******************************************************************************
 */

#ifndef __HW_INIT_H
#define __HW_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"

/* Exported handles ----------------------------------------------------------*/
extern UART_HandleTypeDef  huart3;
extern DMA2D_HandleTypeDef hdma2d;
extern LTDC_HandleTypeDef  hltdc;
extern XSPI_HandleTypeDef  hxspi1;
extern DCMIPP_HandleTypeDef hdcmipp;
extern CACHEAXI_HandleTypeDef hcacheaxi;
extern XSPI_HandleTypeDef hxspi2;
/* Exported functions --------------------------------------------------------*/
void hw_init_all(void);
void MX_XSPI1_Init(void);
void MX_XSPI2_Init (void);
#ifdef __cplusplus
}
#endif

#endif /* __HW_INIT_H */
