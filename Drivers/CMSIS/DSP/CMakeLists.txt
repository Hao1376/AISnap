# ============================================================
# CMSIS-DSP 外层包装 — STM32N6 (Cortex-M55 + Helium)
# ============================================================
# 官方 DSP 内部使用 ${ROOT}/CMSIS/DSP 路径约定，
# 此处将 ROOT 指向 Drivers/ 目录以对齐路径。

set(ROOT ${CMAKE_SOURCE_DIR}/Drivers)
set(DSP ${ROOT}/CMSIS/DSP)

list(APPEND CMAKE_MODULE_PATH ${DSP}/Source)
list(APPEND CMAKE_MODULE_PATH ${DSP})

# ---- Helium 加速 (Cortex-M55 MVE) ----
set(HELIUM  ON CACHE BOOL "Enable Helium MVE acceleration")
set(MVEF    ON CACHE BOOL "Enable MVE-F (float)")
set(MVEI    ON CACHE BOOL "Enable MVE-I (integer)")

# ---- 不用的特性 ----
set(NEON              OFF CACHE BOOL "No NEON on M55")
set(NEONEXPERIMENTAL  OFF CACHE BOOL "")
set(HELIUMEXPERIMENTAL OFF CACHE BOOL "Use stable Helium path")
set(HOST              OFF CACHE BOOL "")
set(DISABLEFLOAT16    OFF CACHE BOOL "")

# ---- 优化选项 ----
set(LOOPUNROLL  ON CACHE BOOL "Loop unrolling")
set(ROUNDING   OFF CACHE BOOL "No rounding (faster)")

# ---- 功能模块：按需开启 ----
# AI眼镜实际用到：矩阵乘法、基本运算、快速数学、统计、支持函数
set(BASICMATH      OFF  CACHE BOOL "")  # abs/add/sub/mult/scale → 手势特征计算
set(MATRIX         OFF  CACHE BOOL "")  # arm_mat_mult_f32 → 推理后处理
set(FASTMATH       OFF  CACHE BOOL "")  # sin/cos/sqrt → 指尖角度计算
set(STATISTICS     OFF  CACHE BOOL "")  # min/max/mean → 手势特征统计
set(SUPPORT        OFF  CACHE BOOL "")  # copy/fill/weighted_sum → 依赖基类

set(COMPLEXMATH    OFF CACHE BOOL "")  # 复数运算 → 不需要
set(CONTROLLER     OFF CACHE BOOL "")  # PID控制器 → 不需要
set(FILTERING      OFF CACHE BOOL "")  # FIR/IIR滤波 → 不需要
set(TRANSFORM      OFF CACHE BOOL "")  # FFT/DCT/MFCC → 砍掉1.5MB查表！
set(SVM            OFF CACHE BOOL "")  # SVM分类器 → 不需要
set(BAYES          OFF CACHE BOOL "")  # 贝叶斯 → 不需要
set(DISTANCE       OFF CACHE BOOL "")  # 距离计算 → 不需要
set(INTERPOLATION  OFF CACHE BOOL "")  # 插值 → 不需要
set(QUATERNIONMATH OFF CACHE BOOL "")  # 四元数 → 不需要

# ---- 表格配置 ----
set(CONFIGTABLE OFF CACHE BOOL "Use default (all tables included)")

# CMSIS-DSP 源码依赖 CMSIS-Core 头文件（cmsis_compiler.h 等）
include_directories(${ROOT}/CMSIS/Include)

# Helium (MVE) 编译器不允许 NEON 风格的向量类型强制转换。
# v1.16.2 的 configDsp.cmake 已通过 target_compile_options 精确处理此标志，
# 这里只需声明 LAXVECTORCONVERSIONS=ON 告知 configDsp 启用该选项。
set(LAXVECTORCONVERSIONS ON CACHE BOOL "Allow lax vector conversions for MVE compatibility")

add_subdirectory(Source)

# 同时对外暴露，让链接 CMSISDSP 的目标也能找到 Core 头文件
target_include_directories(CMSISDSP INTERFACE ${ROOT}/CMSIS/Include)
