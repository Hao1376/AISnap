/**
 ****************************************************************************************************
 * @file        ov5640_dcmipp.h
 * @version     V1.0
 * @date        2025-01-13
 * @brief       OV5640 DCMIPP驱动代码
 ****************************************************************************************************
 * @attention
 * 
 * 
 ****************************************************************************************************
 */

#ifndef __OV5640_DCMIPP_H
#define __OV5640_DCMIPP_H

#include "main.h"

/* 变量导出 */
extern uint8_t ov5640_dcmipp_buf[2 * 1024 * 1024] __attribute__((aligned(16))) __attribute__((section(".EXTRAM")));
extern uint8_t ai_input_buf[192 * 192 * 3] __attribute__((aligned(16))) __attribute__((section(".EXTRAM")));
//extern uint8_t ov5640_dcmipp_buf[1024 * 480 * 2] __attribute__((aligned(16)));
/* 函数声明 */
void ov5640_dcmipp_init(void);  /* 初始化OV5640 DCMIPP */
void ov5640_dcmipp_start(void); /* 启动OV5640 DCMIPP连续采集 */
void ov5640_dcmipp_capture(void); /* 单帧快照采集(防撕裂) */
void ov5640_dcmipp_stop(void);  /* 停止OV5640 DCMIPP传输 */

#endif
