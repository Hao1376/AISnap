/**
 ****************************************************************************************************
 * @file        ov5640_dcmipp.c
 * @version     V1.0
 * @date        2025-01-13
 * @brief       OV5640 DCMIPP驱动代码
 ****************************************************************************************************
 * @attention
 * 
 * 
 ****************************************************************************************************
 */

#include "ov5640_dcmipp.h"

extern DCMIPP_HandleTypeDef hdcmipp;    /* DCMIPP句柄 */

uint8_t ov5640_dcmipp_buf[2 * 1024 * 1024] __attribute__((aligned(16))) __attribute__((section(".EXTRAM")));
uint8_t ai_input_buf[192 * 192 * 3] __attribute__((aligned(16))) __attribute__((section(".EXTRAM")));
//uint8_t ov5640_dcmipp_buf[1024 * 480 * 2] __attribute__((aligned(16)));




/**
 * @brief   初始化OV5640 DCMIPP
 * @param   无
 * @retval  无
 */
void ov5640_dcmipp_init(void)
{
    __HAL_DCMIPP_DISABLE_IT(&hdcmipp, DCMIPP_IT_AXI_TRANSFER_ERROR | DCMIPP_IT_PARALLEL_SYNC_ERROR | DCMIPP_IT_PIPE0_FRAME | DCMIPP_IT_PIPE0_VSYNC | DCMIPP_IT_PIPE0_LINE | DCMIPP_IT_PIPE0_LIMIT | DCMIPP_IT_PIPE0_OVR);
    __HAL_DCMIPP_ENABLE_IT(&hdcmipp, DCMIPP_IT_PIPE0_FRAME);

}

/**
 * @brief   启动OV5640 DCMIPP传输
 * @param   无
 * @retval  无
 */
void ov5640_dcmipp_start(void)
{
    HAL_DCMIPP_PIPE_Start(&hdcmipp, DCMIPP_PIPE0, (uint32_t)ov5640_dcmipp_buf, DCMIPP_MODE_CONTINUOUS);
}

/**
 * @brief   停止OV5640 DCMIPP传输
 * @param   无
 * @retval  无
 */
void ov5640_dcmipp_stop(void)
{
    HAL_DCMIPP_PIPE_Stop(&hdcmipp, DCMIPP_PIPE0);
}
