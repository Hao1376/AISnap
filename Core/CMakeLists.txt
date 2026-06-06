# ============================================================
# Core → 可执行文件（STM32N6_AI_Glasses.elf）
# 作为"基底"，链接所有静态库（BSP / HAL / CMSIS-DSP）
# 包含：main.c、中断处理、系统初始化、Secure NSC、启动文件
# ============================================================

# ---- 源文件 ----
set(CORE_C_SOURCES
    Src/main.c
    Src/hw_init.c
    Src/rif_config.c
    Src/stm32n6xx_it.c
    Src/stm32n6xx_hal_msp.c
    Src/system_stm32n6xx_s.c
    Src/secure_nsc.c
    Src/syscalls.c
    Src/sysmem.c
    Src/stm32n6xx_hal_timebase_tim.c
    
)

# ---- 启动文件（GCC 汇编）----
set(STARTUP_FILE ${CMAKE_SOURCE_DIR}/Device/Startup/startup_stm32n647x0hxq.s)

# ---- include 路径 ----
set(CORE_INCLUDES
    Inc
    # BSP 头文件（main.c 用 #include "./LED/led.h" 等）
    ${CMAKE_SOURCE_DIR}/BSP
    # CMSIS 头文件
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Core/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32N6xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Include
)

# ---- 可执行目标 ----
add_executable(${PROJECT_NAME}
    ${CORE_C_SOURCES}
    ${STARTUP_FILE}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CORE_INCLUDES})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${COMMON_COMPILE_DEFINITIONS})
target_compile_options(${PROJECT_NAME} PRIVATE ${COMMON_COMPILE_OPTIONS})

# ---- 链接静态库 ----
target_link_libraries(${PROJECT_NAME} PRIVATE
    BSP_Lib
    App_Lib          # AI 推理 + 网络模型
    MiddleWare_Lib
    HAL_Lib
    CMSISDSP
)

# ---- 链接脚本 ----
# 默认使用 LRUN（全部代码在内部 RAM），可通过 -DLINKER_SCRIPT=... 切换
if(NOT DEFINED LINKER_SCRIPT)
    set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/Device/LD/STM32N647X0HXQ_LRUN_RAMxspi1_xspi2_weights.ld)
endif()

# GCC (GNU ld) 语法
target_link_options(${PROJECT_NAME} PRIVATE
    -T${LINKER_SCRIPT}
    -Wl,-Map=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.map
)

# ---- 构建后处理：生成 .bin、.dis 反汇编 + 打印大小 ----
# GNU objdump / size 语法
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}>
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJDUMP} -S $<TARGET_FILE:${PROJECT_NAME}> > ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.dis
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Generating .bin, .disassembly and size report"
)