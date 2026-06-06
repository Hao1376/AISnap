cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0123 NEW)
set(BUILD_CMAKE_DIR ${CMAKE_SOURCE_DIR}/tool)

# 工具链: GCC (arm-none-eabi)
message(STATUS "Toolchain: GCC (arm-none-eabi)")
include(${BUILD_CMAKE_DIR}/cmake/arm-none-eabi-toolchain.cmake)

# 编译配置（必须在 toolchain 之后、project() 之前）
include(${BUILD_CMAKE_DIR}/cmake/compile.cmake)

project(STM32N6_AI_Glasses C ASM CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================
# 静态库（按依赖顺序编译，后编译的可以依赖先编译的）
# ============================================================
add_subdirectory(Drivers/STM32N6xx_HAL_Driver)   # HAL 静态库（无外部依赖）
add_subdirectory(Drivers/CMSIS/DSP)               # CMSIS-DSP 静态库
add_subdirectory(BSP)                              # BSP 外设驱动静态库（依赖 HAL）
add_subdirectory(Middlewares)                      # MiddleWare 静态库（中间件 依赖HAL）
add_subdirectory(App)                              # App 应用层静态库（AI推理 + 网络模型，依赖 Middleware + HAL）

# ============================================================
# Core = 可执行文件（"基底"，链接所有静态库）
# ============================================================
add_subdirectory(Core)
