# ============================================================
# 工具链配置 — Cortex-M55 TrustZone Secure (STM32N647X0HXQ)
# 参考: C:\Users\shark\Desktop\Project\tool\cmake\arm-none-eabi-toolchain.cmake
# ============================================================

set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

# 工具链根目录（可通过 -DARM_TOOLCHAIN_PATH=... 覆盖）
set(ARM_TOOLCHAIN_PATH "G:/gcc_toolchain/arm-gnu-toolchain-15.2.rel1-mingw-w64-i686-arm-none-eabi"
    CACHE PATH "ARM GNU Toolchain root directory")

# 编译器（在 project() 之前强制指定）
set(CMAKE_C_COMPILER    ${ARM_TOOLCHAIN_PATH}/bin/arm-none-eabi-gcc.exe     CACHE FILEPATH "" FORCE)
set(CMAKE_CXX_COMPILER  ${ARM_TOOLCHAIN_PATH}/bin/arm-none-eabi-g++.exe     CACHE FILEPATH "" FORCE)
set(CMAKE_ASM_COMPILER  ${ARM_TOOLCHAIN_PATH}/bin/arm-none-eabi-gcc.exe     CACHE FILEPATH "" FORCE)

# 辅助工具
set(CMAKE_OBJCOPY       ${ARM_TOOLCHAIN_PATH}/bin/arm-none-eabi-objcopy.exe)
set(CMAKE_SIZE          ${ARM_TOOLCHAIN_PATH}/bin/arm-none-eabi-size.exe)
set(CMAKE_LINKER        ${ARM_TOOLCHAIN_PATH}/bin/arm-none-eabi-g++.exe)

# 输出后缀
set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

# 禁止 CMake 使用宿主系统的头文件/库
set(CMAKE_FIND_ROOT_PATH            ${ARM_TOOLCHAIN_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# 防止 CMake 尝试在宿主上编译测试程序
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ============================================================
# MCU 专有编译标志
# Cortex-M55 (armv8.1-m.main)，硬件浮点 ABI
# -mfloat-abi=hard: 使用 FPU 硬件寄存器传参，STM32N6 有双精度 FPU
# -mfpu=auto: 由 -mcpu=cortex-m55 自动推导 FPU 类型（FPv5-DP-D16 + Helium MVE）
# -mcmse: 启用 ARMv8-M 安全扩展（secure_nsc.c 需要 CMSE 内联函数）
# ============================================================
set(TARGET_FLAGS "-mcpu=cortex-m55 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -mcmse")

# C 编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS} -Wall -fdata-sections -ffunction-sections -fstack-usage")

# ASM 编译选项（-x assembler-with-cpp 支持预处理指令）
set(CMAKE_ASM_FLAGS "${TARGET_FLAGS} -x assembler-with-cpp -MMD -MP")

# 优化等级
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-O2 -g0")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -g0")

# C++ 编译选项（以 TARGET_FLAGS 为基础，避免重复追加 CMAKE_C_FLAGS）
set(CMAKE_CXX_FLAGS "${TARGET_FLAGS} -Wall -fdata-sections -ffunction-sections -fno-rtti -fno-exceptions -fno-threadsafe-statics")

# 链接器选项（各目标通过 target_link_options 追加 -T 链接脚本）
set(CMAKE_EXE_LINKER_FLAGS "${TARGET_FLAGS} --specs=nano.specs -Wl,--gc-sections")
# 链接数学库（DSP 库依赖 libm）
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")
