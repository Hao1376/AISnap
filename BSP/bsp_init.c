/**
 ******************************************************************************
 * @file           : bsp_init.c
 * @brief          : BSP initialization — display, UART, camera, DCMIPP pipeline
 *
 *   All code extracted verbatim from main.c USER CODE BEGIN 2 block.
 *   No logic or values changed — only organized into single-responsibility
 *   functions.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "bsp_init.h"
#include "./LED/led.h"
#include "./OV5640/ov5640.h"
#include "./RGBLCD/rgblcd.h"
#include "./UART/uart.h"
#include "app_x-cube-ai.h"
#include "hw_init.h"
#include <stdio.h>

/* External variables (defined in main.c) -----------------------------------*/
extern uint16_t img_width;
extern uint16_t img_height;
extern uint16_t img_x_offset;
extern uint16_t img_y_offset;
extern UART_HandleTypeDef UartHandle;
volatile uint16_t Process_OK=0;
/* ========================================================================= */
/*  1. Display — LED + RGB LCD                                               */
/* ========================================================================= */

void bsp_init_display(void) {
  led_init();            /* Initialize LED */
  rgblcd_init();         /* Initialize RGB LCD */
  rgblcd_display_dir(1); /* Landscape */
}

/* ========================================================================= */
/*  2. UART — debug console                                                  */
/* ========================================================================= */

void bsp_init_uart(void) {
  UartHandle = huart3; /* Point STAI UartHandle to USART3 for printf/debug output */
  uart_init(115200);
}

/* ========================================================================= */
/*  3. Camera — OV5640 sensor configuration                                  */
/* ========================================================================= */

void bsp_init_camera(void) {
  /* Sensor init */
  while (ov5640_init()) {
    printf("ov5640 error\r\n");
  }
  ov5640_rgb565_mode();

  /* Image quality */
  ov5640_focus_init(); /* Auto-focus init */
  ov5640_light_mode(0);
  ov5640_color_saturation(3); /* Saturation */
  ov5640_brightness(4);       /* Brightness */
  ov5640_contrast(3);
  ov5640_sharpness(33);
  ov5640_focus_constant();

  /* Output resolution */
  img_width = rgblcddev.width;
  img_height = rgblcddev.height;
  if (img_width >= 800) {
    img_width = 640;
    img_height = 480;
  }
  img_x_offset = (rgblcddev.width - img_width) >> 1;
  img_y_offset = (rgblcddev.height - img_height) >> 1;
  ov5640_outsize_set(4, 0, img_width, img_height); /* Output 640x480 */
}

/* ========================================================================= */
/*  4. Pipeline — DCMIPP capture + crop → AI input                           */
/* ========================================================================= */

void bsp_init_pipeline(void)
{
  /* Start camera capture (Pipe 0: 640x480 → framebuffer) */
  ov5640_dcmipp_init();  /* Enable Pipe 0 frame interrupt */
  ov5640_dcmipp_start(); /* DCMIPP auto, continuous DMA camera data to
                            ov5640_dcmipp_buf */

  /* Crop pipeline: Pipe 2 downsizes 640x480 → 192x192 */
  DCMIPP_DownsizeTypeDef dsCfg = {0};
  dsCfg.HSize = 192;                    /* Target width */
  dsCfg.VSize = 192;                    /* Target height */
  dsCfg.HRatio = 0;                     /* No ratio (use div factor) */
  dsCfg.VRatio = 0;                     /* No ratio */
  dsCfg.HDivFactor = (640 * 128) / 192; /*  427 */
  dsCfg.VDivFactor = (480 * 128) / 192; /*  320 */
  HAL_DCMIPP_PIPE_SetDownsizeConfig(&hdcmipp, DCMIPP_PIPE2, &dsCfg);
  HAL_DCMIPP_PIPE_EnableDownsize(&hdcmipp, DCMIPP_PIPE2);
  HAL_DCMIPP_PIPE_Start(&hdcmipp, DCMIPP_PIPE2, (uint32_t)stai_input[0],DCMIPP_MODE_CONTINUOUS);
  __HAL_DCMIPP_ENABLE_IT(&hdcmipp,DCMIPP_IT_PIPE2_FRAME); /* Trigger AI processing */
}
/* ========================================================================= */
/*  4. Stub Function for Hand Detection AI Testing                           */
/* ========================================================================= */
void bsp_stun_func(void) {}

/* ========================================================================= */
/*  Convenience wrapper                                                      */
/* ========================================================================= */

void bsp_init_all(void) {
  bsp_init_display();
  bsp_init_uart();
  bsp_init_camera();
  bsp_init_pipeline();
}

/**
 * @brief   HAL DCMIPP pipe frame event callback
 * @param   pHdcmipp: DCMIPP handle pointer
 * @param   Pipe: DCMIPP pipe number
 * @retval  None
 *
 * @warning PIPE0 callback renders the framebuffer to LTDC.
 *          PIPE2 callback currently runs aiRun() in ISR context.
 *          When migrating to FreeRTOS, replace with
 *          xSemaphoreGiveFromISR() to signal the AI task.
 */
void HAL_DCMIPP_PIPE_FrameEventCallback(DCMIPP_HandleTypeDef *hdcmipp,
                                        uint32_t Pipe) {
  if (hdcmipp->Instance == DCMIPP)
  {
    if (Pipe == DCMIPP_PIPE0)
	{
      rgblcd_color_fill(
          img_x_offset, img_y_offset, img_x_offset + img_width - 1,
          img_y_offset + img_height - 1, (uint16_t *)ov5640_dcmipp_buf);
    }
    else if (Pipe == DCMIPP_PIPE2)
    {
 	    __HAL_DCMIPP_DISABLE_IT(hdcmipp, DCMIPP_IT_PIPE2_FRAME); /* 暂停 Pipe2 中断，防止 DMA 覆盖 */
      	Process_OK=1;
    }
  }
}
