
/**
 ******************************************************************************
 * @file    app_x-cube-ai.c
 * @author  X-CUBE-AI C code generator
 * @brief   AI program body
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

/**
 * Description
 * Minimum template to show how to use the Neural-ART Embedded Client API
 *          Re-target of the printf function is out-of-scope.
 *
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* System headers */
#include "app_x-cube-ai.h"
#include "npu_init.h"
#include "stai.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* USER CODE BEGIN includes */
#include <math.h>
/* USER CODE END includes */

/* Uncomment to use test image instead of camera */
// #define USE_TEST_IMAGE

/* IO buffers ----------------------------------------------------------------*/
#include "pd_anchors.h"

static Detection detections[MAX_DETECTIONS] = {0}; // 工作数组
static Detection results[MAX_DETECTIONS] = {0};    // 输出数组
static uint32_t Detection_count = 0;
/* Input defs ----------------------------------------------------------------*/
#include "aiTestUtility.h"
/**

// Array to store the data of the input tensor
stai_ptr data_ins[] = {
};
*/

/* Output defs
 * ----------------------------------------------------------------*/

/**

// c-array to store the data of the output tensor
stai_ptr data_outs[] = {
};
*/

/* Global byte buffer to save instantiated C-model network context */
STAI_NETWORK_CONTEXT_DECLARE(network_context, STAI_NETWORK_CONTEXT_SIZE)

/* Activations buffers -------------------------------------------------------*/
#define CONF_THRESHOLD 0.5f
/* Entry points --------------------------------------------------------------*/

/* Array of pointer to manage the model's input/output tensors */
stai_size in_length, out_length;
stai_ptr stai_input[STAI_NETWORK_IN_NUM];
stai_ptr stai_output[STAI_NETWORK_OUT_NUM];

/*
 * Bootstrap
 */
int aiInit(void) {
  stai_return_code ret_code;

  /* 1: Initialize runtime library */
  ret_code = stai_runtime_init();

  /* 2: Initialize network model context */
  ret_code = stai_network_init(network_context);
  ret_code = stai_network_get_inputs(network_context, stai_input, &in_length);

  ret_code =
      stai_network_get_outputs(network_context, stai_output, &out_length);

  return 0;
}

int aiDeinit(void) {
  stai_return_code ret_code;

  /* 1: Deinitialize network model context */
  ret_code = stai_network_deinit(network_context);

  /* 2: Deinitialize runtime library */
  ret_code = stai_runtime_deinit();

  return 0;
}

/*
 * Run inference
 */
stai_return_code aiRun() {
  stai_return_code ret_code;

  /** Profiling code to calculate the inference time of the model. You can
   * remove it if not needed */
  static uint32_t inference_nb = 0;
  static uint32_t total_cycles = 0;
  uint32_t start_tick, end_tick, end_dwt = 0;
  struct dwtTime t;
  cyclesCounterInit();

  LC_PRINT("---- Inference number %" PRIu32 " ----\r\n", inference_nb);
  LC_PRINT("Results for network \"%s\"\r\nRunning...\r\n",
           STAI_NETWORK_C_MODEL_NAME);
  cyclesCounterStart();       // Start timing
  start_tick = HAL_GetTick(); // Record start time

  /* Perform the inference */
  ret_code =
      stai_network_run(network_context, STAI_MODE_SYNC); // Start inference
  if (ret_code != STAI_SUCCESS) {
    ret_code = stai_network_get_error(network_context);
    LC_PRINT("Inference failed with error code %s\r\n",
             stai_get_return_code_name(ret_code));
  };
  /** End of inference */

  /** Continue profiling */
  end_dwt = cyclesCounterEnd(); // Stop timing
  total_cycles += end_dwt;
  end_tick = HAL_GetTick(); // Record stop time
  dwtCyclesToTime(end_dwt, &t);

  LC_PRINT(" duration DWT    : %d.%03d ms\r\n", t.s * 1000 + t.ms, t.us);
  LC_PRINT(" duration SysTick: %" PRIu32 " ms\r\n", end_tick - start_tick);
  LC_PRINT(" CPU cycles      : %" PRIu32 "\r\n", end_dwt);
  LC_PRINT(" CPU cycles (avg): %" PRIu32 "\r\n", total_cycles / ++inference_nb);
  return ret_code;
}

/**
 * @brief  Data preprocessing - fill AI input tensor from camera buffer.
 *
 * @return int
 */
int acquire_and_process_data() {

  uint8_t *src = (uint8_t *)stai_input[0]; // 当前是 uint8_t 数据
  float *dst = (float *)stai_input[0];     // 要转成 float32

  // 此处要从后往前转换
  int total_pixels = 192 * 192 * 3; // 110592

  for (int i = total_pixels - 1; i >= 0; i--) {
    dst[i] = (src[i] / 127.5f) - 1.0f;
  }

  return 0;
}

/**
 * @brief  Process inference results.
 *
 * @return int
 */
/*stai output 0 置信度 2016个float32
 stai output 1 回归值 2016*18个float32
每个锚框有 18 个回归值：
索引 0-1：边界框中心偏移 (dx, dy)
索引 2-3：边界框宽高 (w, h)
索引 4-17：7 个手部关键点 × 2 坐标 (x, y)
*/

void IOU_Calc(pDetection data, uint32_t length) {
  for (int i = 0; i < length; i++) {
    if (data[i].score == 0)
      continue;

    // A 框左上角 + 右下角
    float aix1 = data[i].cx - data[i].w / 2;
    float aiy1 = data[i].cy - data[i].h / 2;
    float aix2 = data[i].cx + data[i].w / 2;
    float aiy2 = data[i].cy + data[i].h / 2;
    float area_i = data[i].w * data[i].h;

    for (int j = i + 1; j < length; j++) {
      if (data[j].score == 0)
        continue;

      // B 框左上角 + 右下角
      float bjx1 = data[j].cx - data[j].w / 2;
      float bjy1 = data[j].cy - data[j].h / 2;
      float bjx2 = data[j].cx + data[j].w / 2;
      float bjy2 = data[j].cy + data[j].h / 2;

      float area_j = data[j].w * data[j].h;

      // 交集
      float x1 = fmaxf(aix1, bjx1);
      float y1 = fmaxf(aiy1, bjy1);
      float x2 = fminf(aix2, bjx2);
      float y2 = fminf(aiy2, bjy2);
      float inter = fmaxf(0, x2 - x1) * fmaxf(0, y2 - y1); // 交集面积
      if (inter <= 0)
        continue;

      // IoU 交并比
      float iou = inter / (area_i + area_j - inter);

      if (iou > 0.5f) // 符合
      {
        if (data[i].score < data[j].score) {
          data[i].score = 0;
          break; // i 被抑制
        } else {
          data[j].score = 0; // j 被抑制
        }
      }
    }
  }
}

int post_process() {
  float *score = (float *)stai_output[0]; // 2016 个置信度
  float *regs = (float *)stai_output[1];  // 2016×18 个回归值

  uint32_t dected = 0;

  // 1. 置信度阈值转换，利用sigmod反函数
  float logit_threshold = -logf(1.0f / CONF_THRESHOLD - 1.0f);
  // 2. 寻找突破置信度阈值的所有锚点偏移量与关键骨骼点
  for (int i = 0; i < 2016; i++) 
  {
    // 我们要保留这个锚定框
    if (score[i] >= logit_threshold) 
	{
      float anchor_x = g_Anchors[i].x; // 锚框中心 x（归一化）
      float anchor_y = g_Anchors[i].y; // 锚框中心 y（归一化）
      // 解码边界框中心 = 锚框中心 * 192（还原在像素上的位置） +
      // （加上偏移量），再归一化
      detections[dected].cx = (anchor_x * 192.0f + regs[i * 18 + 0]) / 192.0f;
      detections[dected].cy = (anchor_y * 192.0f + regs[i * 18 + 1]) / 192.0f;
      detections[dected].w = regs[i * 18 + 2] / 192.0f;
      detections[dected].h = regs[i * 18 + 3] / 192.0f;
      for (int j = 0; j < 7; j++) {
        detections[dected].kps[j][0] = (anchor_x * 192.0f + regs[i * 18 + 4 + j * 2]) / 192.0f;
        detections[dected].kps[j][1] = (anchor_y * 192.0f + regs[i * 18 + 4 + j * 2 + 1]) / 192.0f;
      }
      detections[dected].score = score[i];
      dected++;
    }
  }
  // 3. 进行非极大值抑制（NMS）
  IOU_Calc(detections, dected);
  Detection_count = 0;
  for (int i = 0; i < dected; i++) 
  {
    if (detections[i].score != 0) 
	{
      results[Detection_count++] = detections[i];
    }
  }
  return Detection_count;
}
/**
 * @brief 外部接口，负责拿去最终锚框和锚框个数
 *
 * @param count
 * @return Detection*
 */
Detection *get_results(uint32_t     *count) {
  if (count!=NULL) 
  {
    *count = Detection_count;
  }
  return results;
}

/* Entry points --------------------------------------------------------------*/

void STM32CubeAI_Studio_AI_Init(void) {
  aiPreInitialize();
  aiInit();
  /* USER CODE BEGIN init */
  /* USER CODE END init */
}

/*
 * NOTE: main_loop() and STM32CubeAI_Studio_AI_Process() were removed.
 * The pipeline loop is driven by main.c → while(1) calling
 * acquire_and_process_data() → aiRun() → post_process() directly.
 *
 * When migrating to FreeRTOS, the loop will be replaced by task-based
 * scheduling.  See pipeline.h for the planned task interface.
 */

void STM32CubeAI_Studio_AI_Deinit(void) { aiDeinit(); }

#ifdef __cplusplus
}
#endif
