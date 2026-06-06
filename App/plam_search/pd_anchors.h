/**
  ******************************************************************************
  * @file           : pd_anchors.h
  * @brief          : Palm detection anchor points - header
  ******************************************************************************
  */
#ifndef __PD_ANCHORS_H__
#define __PD_ANCHORS_H__

#include "arm_math.h"

typedef struct {
  float32_t x;
  float32_t y;
} pd_pp_point_t;

extern pd_pp_point_t g_Anchors[2016];

#endif /* __PD_ANCHORS_H__ */
