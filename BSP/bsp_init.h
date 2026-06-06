/**
 ******************************************************************************
 * @file           : bsp_init.h
 * @brief          : Unified BSP initialization entry point
 *
 *   Layered init sequence (call in this order):
 *     1. bsp_init_display()   — LED + RGB LCD
 *     2. bsp_init_uart()      — USART3 debug output
 *     3. bsp_init_camera()    — OV5640 sensor configuration
 *     4. bsp_init_pipeline()  — DCMIPP crop pipeline → AI input
 *
 *   bsp_init_all() is the convenience wrapper that chains them all.
 *
 *   FreeRTOS note:
 *     Steps 1-3 run pre-scheduler. Step 4 (pipeline start + IRQ enable)
 *     should move to a dedicated camera task after FreeRTOS migration.
 ******************************************************************************
 */

#ifndef __BSP_INIT_H
#define __BSP_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/* ── Individual init steps ── */
void bsp_init_display(void);
void bsp_init_uart(void);
void bsp_init_camera(void);
void bsp_init_pipeline(void);

/* ── Convenience: chain all in correct order ── */
void bsp_init_all(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_INIT_H */
