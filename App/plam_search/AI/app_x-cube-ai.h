/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_AI_H
#define __APP_AI_H
#ifdef __cplusplus
extern "C" {
#endif
/**
 ******************************************************************************
 * @file    app_x-cube-ai.h
 * @author  STM32Cube AI Studio C code generator
 * @brief   AI entry function definitions
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
/* Includes ------------------------------------------------------------------*/
#include "ai_datatypes_defines.h"
#include "ll_aton_runtime.h"
#include "stai.h"
#include "stai_network.h"
#include "user_init.h"


/* IO buffers ----------------------------------------------------------------*/
extern stai_ptr stai_input[STAI_NETWORK_IN_NUM];
extern stai_ptr stai_output[STAI_NETWORK_OUT_NUM];
typedef struct {
  float cx, cy;    // 中心点
  float w, h;      // 宽高
  float kps[7][2]; // 7 个关键点
  float score;     // 置信度（用于 NMS 排序）
} Detection, *pDetection;
#define MAX_DETECTIONS 100
void STM32CubeAI_Studio_AI_Init(void);
void STM32CubeAI_Studio_AI_Process(void);
void STM32CubeAI_Studio_AI_Deinit(void);
Detection *get_results(uint32_t *count);
/* Function prototypes -------------------------------------------------------*/
int acquire_and_process_data(void);
int post_process(void);
stai_return_code aiRun(void);

/* USER CODE BEGIN includes */
/* USER CODE END includes */

#ifdef __cplusplus
}
#endif
#endif /*__STMicroelectronics_ST_EDGE_AI_4.0.0-20500 359356bb0_H */
