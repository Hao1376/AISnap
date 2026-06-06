

file(GLOB_RECURSE HAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Src/*.c)

# 排除模板文件（_template.c 只是 ST 提供的参考模板，不应编译）
list(FILTER HAL_SOURCES EXCLUDE REGEX ".*_template.*\\.c$")
# 排除 LL 低层驱动（纯 HAL 使用不需要 LL .c 文件，链接时缺哪个再加回）
list(FILTER HAL_SOURCES EXCLUDE REGEX ".*_ll_.*\\.c$")

add_library(HAL_Lib STATIC ${HAL_SOURCES})

target_include_directories(HAL_Lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

target_include_directories(HAL_Lib PRIVATE
    # hal_conf.h 在 Core/Inc 下（用户项目定义，非 HAL 自带）
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Core/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32N6xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
)

target_compile_definitions(HAL_Lib PRIVATE ${COMMON_COMPILE_DEFINITIONS})
target_compile_options(HAL_Lib PRIVATE ${COMMON_COMPILE_OPTIONS})
