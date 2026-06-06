# ============================================================
# 工具链配置 — ARM Compiler 6 (armclang) for Cortex-M55
# 路径: C:\Program Files\ArmCompilerforEmbedded6.24\bin
# ============================================================

set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

# 工具链根目录（可通过 -DARMCLANG_TOOLCHAIN_PATH=... 覆盖）
set(ARMCLANG_TOOLCHAIN_PATH "C:/Users/shark/AppData/Local/Keil_v5/ARM/ARMCLANG/bin"
    CACHE PATH "ARMCLang toolchain bin directory")

# ---- 编译器 ----
# armclang 是 LLVM/Clang 基的 C/C++ 编译器
# 同时用 armclang 处理汇编（兼容 GCC 语法的 .s 文件）
set(CMAKE_C_COMPILER    ${ARMCLANG_TOOLCHAIN_PATH}/armclang.exe     CACHE FILEPATH "" FORCE)
set(CMAKE_CXX_COMPILER  ${ARMCLANG_TOOLCHAIN_PATH}/armclang.exe     CACHE FILEPATH "" FORCE)
set(CMAKE_ASM_COMPILER  ${ARMCLANG_TOOLCHAIN_PATH}/armclang.exe     CACHE FILEPATH "" FORCE)

# ---- 辅助工具 ----
set(CMAKE_OBJCOPY       ${ARMCLANG_TOOLCHAIN_PATH}/fromelf.exe)     # ELF → bin/hex 转换
#set(CMAKE_OBJDUMP      ${ARMCLANG_TOOLCHAIN_PATH}/fromelf.exe)     # 反汇编（fromelf --text -c）
set(CMAKE_SIZE          ${ARMCLANG_TOOLCHAIN_PATH}/fromelf.exe)     # 查看大小（fromelf --text -z）
# CMAKE_LINKER 设为 armclang.exe（编译器驱动），让 armclang 负责链接
# armclang 作为链接驱动时：
#   1. 正确处理 --target/-mcpu 等编译器选项
#   2. -Wl,xxx 前缀的标志传递给 armlink
#   3. 项目使用的 .ld 链接脚本通过 -Wl,--link_script= 传递
set(CMAKE_LINKER        ${ARMCLANG_TOOLCHAIN_PATH}/armclang.exe    CACHE FILEPATH "" FORCE)
set(CMAKE_AR            ${ARMCLANG_TOOLCHAIN_PATH}/armar.exe)

# ---- 输出后缀 ----
set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

# ---- 禁止 CMake 使用宿主系统头文件/库 ----
set(CMAKE_FIND_ROOT_PATH            ${ARMCLANG_TOOLCHAIN_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# 防止 CMake 尝试在宿主上编译测试程序
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ============================================================
# MCU 编译标志 — Cortex-M55 (armv8.1-m.main)
# armclang 需要 --target 指定目标三元组
# ============================================================
set(TARGET_FLAGS "--target=arm-arm-none-eabi -mcpu=cortex-m55 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -mcmse")

# ---- C 编译选项 ----
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS} -W -fdata-sections -ffunction-sections")

# ---- ASM 编译选项（armclang -x assembler-with-cpp 兼容 GCC 语法 .s 文件）----
set(CMAKE_ASM_FLAGS "${TARGET_FLAGS} -x assembler-with-cpp -MMD -MP")

# ---- 优化等级 ----
set(CMAKE_C_FLAGS_DEBUG     "-O0 -g")
set(CMAKE_C_FLAGS_RELEASE   "-O2 -g0")
set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -g0")

# ---- C++ 编译选项 ----
set(CMAKE_CXX_FLAGS "${TARGET_FLAGS} -W -fdata-sections -ffunction-sections -fno-rtti -fno-exceptions -fno-threadsafe-statics")

# ---- 链接器选项 ----
# armclang 作为链接驱动，内部调用 armlink
# --target 等由 armclang 处理，-Wl, 前缀的标志传递给 armlink
# --library_type=microlib 等效于 GCC 的 --specs=nano.specs
# --remove 等效于 GCC 的 --gc-sections
# ARM Compiler 6 的数学库是内置的，不需要 -lm
set(CMAKE_EXE_LINKER_FLAGS "${TARGET_FLAGS} -Wl,--library_type=microlib -Wl,--remove" CACHE STRING "" FORCE)

# ARM Compiler 6 的数学库是内置的，不需要 -lm（该标志是 GCC 专有）
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")

# ---- 响应文件标志 ----
# CMSIS-DSP 有大量 .o 文件，Ninja 生成器会自动使用响应文件。
# 默认 CMAKE_LINKER=armclang.exe 时，CMake 会用 "-Xlinker --via=" 作为响应文件标志，
# 但 armar.exe（归档工具）不认识 -Xlinker，只认 "--via="。
# 所有 ARM 工具（armclang / armar / armlink）都原生支持 --via= 语法，直接覆盖即可。
# 注意：必须用 CACHE FORCE 防止 CMake 平台模块（Windows-ARMClang.cmake）覆盖此设置。
set(CMAKE_C_RESPONSE_FILE_FLAG    "--via="  CACHE STRING "Response file flag for C (armclang/armar)" FORCE)
set(CMAKE_CXX_RESPONSE_FILE_FLAG  "--via="  CACHE STRING "Response file flag for CXX (armclang/armar)" FORCE)

# ---- 静态库归档规则 ----
# CMAKE_LINKER=armclang 时，CMake 可能把 -Xlinker 传给 armar，导致失败
# 显式指定归档规则，移除 <LINK_FLAGS>，仅使用 armar 原生选项
#（Ninja 生成器不使用此变量，而是用上述 RESPONSE_FILE_FLAG；此处为 Makefile 生成器保留）
set(CMAKE_C_ARCHIVE_CREATE   "<CMAKE_AR> qc <TARGET> <OBJECTS>")
set(CMAKE_C_ARCHIVE_APPEND   "<CMAKE_AR> q  <TARGET> <OBJECTS>")
set(CMAKE_C_ARCHIVE_FINISH   "<CMAKE_RANLIB> <TARGET>")
set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> qc <TARGET> <OBJECTS>")
set(CMAKE_CXX_ARCHIVE_APPEND "<CMAKE_AR> q  <TARGET> <OBJECTS>")
set(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> <TARGET>")

# ============================================================
# 标识当前工具链（供 Core/CMakeLists.txt 等判断）
# ============================================================
set(TOOLCHAIN_IS_ARMCLANG TRUE CACHE INTERNAL "Using ARMCLang toolchain")
