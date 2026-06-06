# ============================================================
# 编译选项 & 公共宏 — STM32N647X0HXQ
# ============================================================

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# C/C++ 标准
set(CMAKE_C_STANDARD          17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS        ON)
set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        ON)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m55)



# 输出目录 —— 统一到 out/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)

message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message(STATUS "Output -> ${CMAKE_SOURCE_DIR}/out")

# 公共编译宏
set(COMMON_COMPILE_DEFINITIONS
    USE_HAL_DRIVER
    STM32N647xx
    DEBUG
)

# 公共编译选项
set(COMMON_COMPILE_OPTIONS
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
)