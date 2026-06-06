/**
 ******************************************************************************
 * @file           : pipeline.h
 * @brief          : Camera → AI → Display pipeline interface
 *
 * This header defines the pipeline API and provides FreeRTOS-ready
 * task entry points for future migration from the superloop.
 *
 * Pipeline flow:
 *   DCMIPP PIPE0 (640×480 → framebuffer) → DCMIPP PIPE2 (192×192 → ai_input_buf)
 *     → aiRun() → post_process()
 *
 * FreeRTOS migration roadmap (commented out below):
 *   1. Uncomment task prototypes and handle declarations
 *   2. Create App/plam_search/tasks/ with ai_task.c, camera_task.c, display_task.c
 *   3. Replace while(1) in main() with task creation + vTaskStartScheduler()
 *   4. Replace ISR direct aiRun() call with xSemaphoreGiveFromISR()
 ******************************************************************************
 */

#ifndef __PIPELINE_H
#define __PIPELINE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* -------------------------------------------------------------------------- */
/*  Pipeline state                                                            */
/* -------------------------------------------------------------------------- */
typedef enum
{
  PIPELINE_STATE_IDLE = 0,
  PIPELINE_STATE_RUNNING,
  PIPELINE_STATE_ERROR,
} pipeline_state_t;

/* -------------------------------------------------------------------------- */
/*  Pipeline lifecycle (bare-metal, compatible with current superloop)        */
/* -------------------------------------------------------------------------- */

/**
 * @brief  Start the camera → AI → display pipeline.
 * @note   Currently runs as a superloop in main().
 *         When migrating to FreeRTOS, this will be replaced by task creation.
 */
void pipeline_start(void);

/**
 * @brief  Stop the pipeline.
 */
void pipeline_stop(void);

/**
 * @brief  Get current pipeline state.
 */
pipeline_state_t pipeline_get_state(void);

/* -------------------------------------------------------------------------- */
/*  FreeRTOS-ready handles (uncomment when migrating to RTOS)                 */
/* -------------------------------------------------------------------------- */

/*
 * Semaphore/queue handles for inter-task communication.
 * Uncomment and use when FreeRTOS is integrated.
 *
 * extern SemaphoreHandle_t xFrameReadySem;   // PIPE0 → display task
 * extern SemaphoreHandle_t xAIReadySem;       // PIPE2 → AI task
 * extern QueueHandle_t     xAIResultQueue;    // AI task → display task
 */

/*
 * Task entry points for FreeRTOS migration.
 * Uncomment and implement in dedicated task files when RTOS is added.
 *
 * void vCameraTask(void *pvParameters);    // Priority: high  - capture + preprocess
 * void vAITask(void *pvParameters);        // Priority: medium - inference
 * void vDisplayTask(void *pvParameters);   // Priority: low   - LTDC refresh
 */

#ifdef __cplusplus
}
#endif

#endif /* __PIPELINE_H */
