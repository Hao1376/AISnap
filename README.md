# AISnap —— 基于 STM32N6 的 Edge AI 视觉事件采集终端

一个运行在 STM32N647 上的边缘 AI 项目。

系统通过 OV5640 摄像头实时采集图像，在 STM32N6 内置 NPU 上运行手掌检测模型，并基于关键点进行手势识别。当检测到指定手势后，系统自动完成事件触发、截图存储、日志记录以及云端上传。

项目覆盖：

- Edge AI 推理部署
- DCMIPP 视频采集
- LTDC 图像显示
- HyperRAM / NOR Flash 管理
- FreeRTOS 多任务架构
- WiFi 通信
- 事件驱动系统设计

> ⚠️ 当前已完成视觉链路验证，事件系统与音频链路开发中，预计于 2026 年 7 月完成第二阶段开发。

## 关于 STM32N6

STM32N6 是 STMicroelectronics 面向 Edge AI 推出的新一代高性能 MCU 系列，也是 STM32 家族首款集成 NPU 的产品。该系列于 2024 年 STM32 Summit 正式发布。

AISnap 使用的 STM32N647 基于 Arm Cortex-M55 内核，采用 Armv8.1-M 架构，支持 Helium（MVE）向量扩展指令集，相比传统 Cortex-M4/M7 在 DSP 运算、AI 前后处理等场景具有更高效率。

STM32N6 同时集成了 ST 自研的 Neural-ART NPU，可提供最高约 600 GOPS 的 AI 推理能力，并配备专用视觉处理链路、4.2MB 片上 SRAM、高速 XSPI 外部存储接口以及图形加速单元，专门面向机器视觉、多媒体和边缘 AI 应用设计。

与传统 MCU 相比，STM32N6 更接近“MCU 与 MPU 的融合体”：既保留了 MCU 的实时性和快速启动特性，又具备运行复杂视觉 AI 应用所需的算力和存储架构。

## 项目背景

STM32N6 发布时间较短，相关生态仍处于发展阶段。
RIF（Resource Isolation Framework）、TrustZone、Neural-ART Runtime 等模块公开资料较少，很多问题需要结合官方论坛、参考手册以及实际调试进行定位和验证，完整开发链路的开源项目较少。

AISnap 旨在探索 STM32N6 在边缘 AI 场景下的完整开发流程，从摄像头采集、视觉推理、事件触发到云端协同，验证 MCU 平台构建 AI 应用系统的可行性，而不仅仅是完成单个 AI 模型的部署。
---

## Demo Pipeline

```text
OV5640 Camera
        │
        ▼
Palm Detection + Keypoints
        │
        ▼
7 Keypoints
        │
        ▼
Gesture Classification
        │
        ▼
Event Trigger
        │
 ┌──────┼──────┐
 ▼      ▼      ▼
BMP    CSV    Cloud
Save   Log   Upload
```

---

## 硬件平台


主控：

- STM32N647
- Arm Cortex-M55 @ 600MHz
- Neural-ART NPU（最高约 600 GOPS）

外设：

- OV5640 RGB Camera
- RGB LCD (LTDC)
- XSPI1 HyperRAM
- XSPI2 NOR Flash
- ESP32 WiFi Module
---

## 技术亮点

本项目重点验证 STM32N6 在视觉 AI 场景下的完整软件栈。

### Edge AI

- STM32N6 NPU 部署 YOLO 模型
- STM32Cube.AI / STAI Runtime
- INT8 量化推理
- 7 关键点手掌检测 + 手势分类

### Vision Pipeline

- OV5640 摄像头
- DCMIPP 双管道：Pipe0 实时预览 / Pipe2 AI 输入
- 硬件同步帧采集

### Graphics

- LTDC 双层显示
- ARGB1555 透明叠加
- DMA2D 硬件加速
- 检测框实时绘制

### Memory Architecture

- XSPI1 HyperRAM：帧缓冲
- XSPI2 NOR Flash XIP：AI 权重 Memory-Mapped
- 链接脚本分级管理

### RTOS

- FreeRTOS 多任务架构
- Queue / TaskNotify / Mutex
- ISR → 任务异步通知

---

## 任务架构

| 任务        | 优先级 | 触发方式                | 职责                         |
| ----------- | :----: | ----------------------- | ---------------------------- |
| CameraTask  |   高   | DCMIPP ISR → TaskNotify | 帧同步                       |
| AITask      |   高   | CameraTask → TaskNotify | NPU 推理 + 后处理 + 手势分类 |
| DisplayTask |   中   | AITask → Queue          | Layer2 叠加绘制              |
| CommandTask |   中   | AITask → Queue          | 事件融合 + 状态机            |
| StorageTask |   低   | CommandTask → Queue     | BMP + CSV 存储               |
| CommTask    |   低   | CommandTask → Queue     | ESP32 AT + MQTT 云端上传     |

通信机制完全基于 FreeRTOS 原生 IPC：

| 机制       | 用途                                   |
| ---------- | -------------------------------------- |
| TaskNotify | ISR → 任务快速通知（DCMIPP 帧同步）    |
| Queue      | 任务间数据传递（检测结果、事件、命令） |
| Mutex      | 共享资源保护（LCD 帧缓冲、事件链表）   |

---

## 内存布局

链接脚本分 Debug 和 Release 两套：

| 模式          | 链接脚本                         | 说明                        |
| ------------- | -------------------------------- | --------------------------- |
| Debug (LRUN)  | `LRUN_RAMxspi1.ld`               | 全在内部 RAM，方便调试      |
| LRUN + XSPI2  | `LRUN_RAMxspi1_xspi2_weights.ld` | 权重放 NOR Flash XIP        |
| Release (ROM) | `ROMxspi2_RAMxspi1.ld`           | 代码 + 权重放 NOR Flash XIP |

Debug 模式下内部 RAM 用量：

| 区域             | 大小                    |
| ---------------- | ----------------------- |
| `.text` (代码)   | ~1.65MB                 |
| `.rodata` (权重) | ~1.1MB                  |
| `.bss/.data`     | ~100KB                  |
| heap + stack     | ~150KB                  |
| **总计**         | **~3.0MB / 2MB → 超了** |

> 这就是为什么要把权重和代码段迁到 NOR Flash XIP。`LRUN_RAMxspi1_xspi2_weights.ld` 把 `.rodata` (权重) 映射到 XSPI2 地址空间 (`0x92000000`)，释放出 ~1.1MB 内部 RAM。

> 链接脚本：[`Device/LD/`](Device/LD/)

---

## 构建

环境：`arm-none-eabi-gcc` + CMake 3.20+ + Ninja

```bash
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_TOOLCHAIN_FILE=../tool/cmake/arm-none-eabi-toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
ninja
```

AI 权重构建（需要 STM32Cube.AI CLI）：

```bash
cd tool
build_weights.bat    # 调用 stm32ai 命令行工具生成 network.c / network_ecblobs.h
```

---

## 当前进度

### 已完成

- 摄像头采集
- LCD 实时显示
- NPU 推理部署
- 检测框绘制
- HyperRAM 驱动
- NOR Flash 驱动

### 开发中

- 手势规则引擎
- BMP 存储
- CSV 日志
- ESP32 云端上传
- KWS 音频链路

---

## Demo

项目仍在持续开发中。

计划在以下功能完成后补充：

- 手势规则引擎
- BMP 存储
- 云端上传
- KWS 音频链路

届时将提供：

- LCD 实时检测演示
- NPU 推理效果视频
- 完整系统运行录像

---

## 调试工具

- **ST-LINK / J-Link**：HardFault 时读 CFSR、HFSR、栈帧回溯
- **printf 串口**：`USART1` 115200bps
- **STM32CubeMX / Cube.AI**：HAL 配置 + AI 模型部署

---

## License

本项目混合多种许可证：

- 应用层代码 (App/BSP/Core)：MIT
- STM32N6 HAL 驱动：BSD-3-Clause
- FreeRTOS：MIT
- ST AI Runtime / 网络模型：ST SLA0104（仅限 STM32 平台使用）
- CMSIS：Apache 2.0

---

_基于 STM32N647 — Cortex-M55 @600MHz + NPU 600 GOPS_
