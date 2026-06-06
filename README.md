# AISnap —— AI 手势触发的事件采集终端

一个运行在 STM32N647 上的端侧 AI 项目。

系统通过 OV5640 摄像头实时监测，在 STM32N6 内置 NPU 上跑手掌检测与手势识别，用户做出特定手势即可触发「事件」——自动截屏保存 BMP、写入 CSV 日志、通过 WiFi 上传云端。目标是覆盖 AI 视觉识别、手势交互、事件管理、本地存储与无线通信全链路的边缘 AI 平台。

> ⚠️ 项目仍在开发中。核心 AI 推理链路已跑通，多任务架构和上层应用正在构建。

## 项目特点

- STM32N6 NPU 端侧推理（900 GOPS, INT8）
- DCMIPP 双管道（Pipe0 预览 + Pipe2 AI 输入）
- LTDC 双层显示（Layer1 实时画面 + Layer2 ARGB1555 透明叠加）
- XSPI HyperRAM / NOR Flash MemoryMapped 模式
- STM32Cube.AI / STAI Runtime 部署
- FreeRTOS 实时调度
- 规则引擎手势分类（7 关键点几何判别）
- CMake + arm-none-eabi-gcc 构建

---

## 硬件平台

MCU：

STM32N647X0HXQ

- Cortex-M55 @600MHz
- 内置 NPU (Neural-Art Accelerator, 900 GOPS)
- 内部 RAM 2MB
- 内部 Flash 128KB

外设：

| 器件 | 接口 | 用途 |
|------|------|------|
| OV5640 | DCMIPP 8-bit | 摄像头 640×480 RGB565 |
| RGB LCD 800×480 | LTDC | 双层显示 |
| HyperRAM | XSPI1 | 帧缓冲 + AI 缓存 |
| NOR Flash | XSPI2 | AI 权重 MemoryMapped |
| TF Card | SDIO | 存储 (规划中) |
| ES8388 | SAI1 I2S | 音频 (规划中) |
| ESP32 | UART ESP-AT | WiFi (规划中) |

## 调试笔记

> [`DEBUG_NOTES.md`](DEBUG_NOTES.md) — LTDC Layer2 不显示排查 · AI Init HardFault 根因分析 (CFSR/HFSR/栈帧) · 内部 RAM 紧张问题

---

## 技术亮点

### DCMIPP 双管道 + LCD 双层叠加

OV5640 输出一路 640×480 RGB565 给 DCMIPP，DCMIPP 分两路：

- **Pipe0 (主预览)**：直接送 LTDC Layer1，显示实时画面
- **Pipe2 (AI 输入)**：下采样到 192×192，存入 HyperRAM 给 NPU 做推理输入

LTDC 配置了两层：

- **Layer1**：RGB565，显示实时摄像头画面
- **Layer2**：ARGB1555，透明叠加检测框、关键点、状态文字

Layer2 的绘制 API 跟 Layer1 完全共用，只是在 `draw_point` 等函数里加了一个 `LTDC_Layer` 参数。写入前自动做 RGB565 → ARGB1555 转换（bit15=1 不透明，bit15=0 透明）。DMA2D 硬件清零覆盖层，不占 CPU。

```c
// 每帧绘制流程
rgblcd_overlay_clear();                                    // DMA2D 清零
rgblcd_draw_rectangle(Layer2, x1, y1, x2, y2, RED);       // 画边界框
rgblcd_draw_circle(Layer2, kx, ky, 3, GREEN);              // 画关键点
rgblcd_show_string(Layer2, 0, 0, 200, 20, 16, "OK", WHITE); // 写状态文字
```

> 源码：[`rgblcd.c`](BSP/RGBLCD/rgblcd.c) · [`rgblcd.h`](BSP/RGBLCD/rgblcd.h)

### NPU 推理部署

使用 STM32Cube.AI 生成的 STAI Runtime 在 NPU 上跑手掌检测模型。模型是 INT8 全量化，输出 bbox (cx, cy, w, h) + score + 7 个关键点，归一化坐标。

模型权重默认放在 NOR Flash (XSPI2 MemoryMapped)，NPU 通过 AXI 总线直接读取，不占内部 RAM。

当前踩的一个坑是 AI Init 阶段 `LL_ATON_EC_Network_Init_network` 进 HardFault，CFSR = 0x100 (IBUSERR)。排查发现是内部 RAM 紧张：`.text` 段 1.65MB 占 2MB RAM 的 80%，bss/data/heap/stack 挤在剩余 ~400KB 里，ST AI 预编译库栈消耗大，溢出踩坏返回地址。计划将权重和代码段迁到 NOR Flash XIP 解决。

> 源码：[`app_x-cube-ai.c`](App/plam_search/AI/app_x-cube-ai.c) · [`network.c`](App/plam_search/Network/network.c) · [`network_atonbuf.xSPI2.c`](App/plam_search/Network/network_atonbuf.xSPI2.c)

### XSPI 双外设 MemoryMapped

STM32N6 有两路 XSPI（Octal-SPI），都配置成了 MemoryMapped 模式：

- **XSPI1 → HyperRAM**：帧缓冲 (`0x90000000`)、AI 输入/输出缓存。200MHz DTR，读带宽 ~400MB/s。
- **XSPI2 → NOR Flash**：AI 权重存储 (`0x92000000`)。NOR Flash 用 BY25FQ128EL (16MB) 或 MX25UM25645G (32MB)，JEDEC ID 验证通过。

MemoryMapped 的好处是 NPU 和 DMA2D 可以直接从 XSPI 地址空间读数据，不需要 CPU 主动搬运。

> 源码：[`hyperram.c`](BSP/HyperRAM/hyperram.c) · [`norflash.c`](BSP/NORFlash/norflash.c)

### 手势分类规则引擎（设计阶段）

不重新训练模型，而是利用模型输出的 7 个关键点（手腕 + 5 指尖 + 掌心）做几何判别：

```
kps[0]: 手腕 | kps[1]: 拇指指尖 | kps[2]: 食指指尖 | kps[3]: 中指指尖
kps[4]: 无名指指尖 | kps[5]: 小指指尖 | kps[6]: 掌心
```

```c
typedef enum {
    GESTURE_NONE   = 0,
    GESTURE_OPEN   = 1,  // 5 指尖全展开
    GESTURE_FIST   = 2,  // 5 指尖靠近掌心
    GESTURE_V_SIGN = 3,  // 食指+中指伸展，其余靠拢
    GESTURE_POINT  = 4,  // 仅食指伸展
} gesture_t;
```

判别逻辑：计算每个指尖到掌心的距离，与阈值比较。V_SIGN 额外判断食指-中指距离。

## 为什么做这个项目

STM32N6 是 ST 刚出的带 NPU 的 Cortex-M55，市面上几乎找不到完整的开源项目参考。

这个项目的目标不仅是跑通一个 AI Demo，而是从硬件驱动、AI 推理、显示叠加到 RTOS 任务架构，走一遍完整的产品级开发流程。踩过的坑（NPU HardFault、LTDC Layer2 不显示、RAM 不够用）本身就是最有价值的经验。

目前项目还在持续推进中，已完成的部分已经能证明在 STM32N6 上做边缘 AI 全链路的可行性。

## 目录

1. [工程结构](#1-工程结构)
2. [FreeRTOS 任务架构](#2-freertos-任务架构)
3. [内存布局](#3-内存布局)
4. [构建](#4-构建)

---

## 1. 工程结构

```
N65_AI/
├── App/plam_search/          # AI 应用层
│   ├── AI/                   # NPU 初始化、推理封装、权重管理
│   └── Network/              # 网络模型 .c/.h (X-CUBE-AI 生成)
├── BSP/                      # 板级驱动
│   ├── RGBLCD/               # LTDC 双层显示、绘图 API (点/线/矩形/圆/字符)
│   ├── OV5640/               # 摄像头 DCMIPP 双管道采集
│   ├── HyperRAM/             # XSPI1 HyperRAM MemoryMapped
│   ├── NORFlash/             # XSPI2 NOR Flash (多型号自适应)
│   ├── JPEGCODEC/            # 硬件 JPEG 编解码
│   └── LED/                  # WS2812B / GPIO LED
├── Core/                     # 内核
│   ├── Inc/                  # FreeRTOSConfig.h, hw_init.h, HAL 配置
│   └── Src/                  # main.c, 中断服务, HAL MSP, RIF 安全配置
├── Device/                   # 芯片级
│   ├── Startup/              # 启动汇编 (startup_stm32n647x0hxq.s)
│   └── LD/                   # 链接脚本 (LRUN/ROM/XIP 多模式)
├── Drivers/                  # HAL 库
│   ├── STM32N6xx_HAL_Driver/ # STM32N6 HAL
│   └── CMSIS/                # CMSIS Core + DSP
├── Middlewares/              # 中间件
│   ├── FreeRTOS/             # FreeRTOS Kernel
│   └── STAI/Npu/             # ST AI Runtime (ll_aton)
├── tool/                     # 辅助工具
│   ├── build_weights.bat     # 权重构建 (X-CUBE-AI CLI)
│   ├── img2bin.py            # 图片转二进制
│   └── cmake/                # CMake 工具链 (GCC / ARMClang)
├── CMakeLists.txt            # 顶层 CMake
├── PROJECT_PLAN.md           # 详细开发规划 (4 周路线)
└── DEBUG_NOTES.md            # 调试记录
```

## 2. FreeRTOS 任务架构

当前代码跑通了 FreeRTOS 内核，但 main 函数还在用裸机轮询 `Process_OK` 标志位。计划拆分为以下多任务架构：

| 任务 | 优先级 | 触发方式 | 职责 |
|------|:------:|----------|------|
| CameraTask | 高 | DCMIPP ISR → TaskNotify | 帧同步 |
| AITask | 高 | CameraTask → TaskNotify | NPU 推理 + 后处理 + 手势分类 |
| DisplayTask | 中 | AITask → Queue | Layer2 叠加绘制 |
| CommandTask | 中 | AITask/AudioTask → Queue | 命令融合 + 状态机 |
| StorageTask | 低 | CommandTask → Queue | BMP + CSV 存储 |
| CommTask | 低 | CommandTask → Queue | ESP32 AT + MQTT |
| AudioTask | 中 | SAI DMA ISR → TaskNotify | KWS 推理 |

通信机制完全用 FreeRTOS 原生 IPC，不搞全局变量轮询：

| 机制 | 用途 |
|------|------|
| TaskNotify | ISR → 任务快速通知（DCMIPP、SAI DMA） |
| Queue | 任务间数据传递（检测结果、命令、事件） |
| Mutex | 共享资源保护（LCD 帧缓冲、事件链表） |

## 3. 内存布局

链接脚本分了 Debug 和 Release 两套：

| 模式 | 链接脚本 | 说明 |
|------|----------|------|
| Debug (LRUN) | `LRUN_RAMxspi1.ld` | 全在内部 RAM，方便调试 |
| LRUN + XSPI2 | `LRUN_RAMxspi1_xspi2_weights.ld` | 权重放 NOR Flash XIP |
| Release (ROM) | `ROMxspi2_RAMxspi1.ld` | 代码 + 权重放 NOR Flash XIP |

Debug 模式下内部 RAM 用量：

| 区域 | 大小 |
|------|------|
| `.text` (代码) | ~1.65MB |
| `.rodata` (权重) | ~1.1MB |
| `.bss/.data` | ~100KB |
| heap + stack | ~150KB |
| **总计** | **~3.0MB / 2MB → 超了** |

这就是为什么要把权重和代码段迁到 NOR Flash XIP。`LRUN_RAMxspi1_xspi2_weights.ld` 把 `.rodata` (权重) 映射到 XSPI2 地址空间 (`0x92000000`)，释放出 ~1.1MB 内部 RAM。

> 链接脚本：[`Device/LD/`](Device/LD/)

## 4. 构建

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

## 调试工具

- **ST-LINK / J-Link**：HardFault 时读 CFSR、HFSR、栈帧回溯
- **printf 串口**：`USART1` 115200bps
- **Source Insight**：代码导航
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

_基于 STM32N647 — Cortex-M55 @600MHz + NPU 900 GOPS_
