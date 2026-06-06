# ============================================================
#  Middle Ware 中间层开关
# ============================================================

set(MiddleWare_FreeRTOS   ON CACHE BOOL "运行FreeRTOS")
set(MiddleWare_STAI       ON CACHE BOOL "运行STAI")
# ============================================================
# 收集已启用的源文件
# ============================================================
set(MiddleWare_SOURCES "")

macro(MiddleWare_add COMPONENT)
    if(MiddleWare_${COMPONENT})
        file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${COMPONENT}/*.c)    #启动递归搜索
        list(APPEND MiddleWare_SOURCES ${SRC_FILES})
        message(STATUS "  MiddleWare: ${COMPONENT} (ON)")
    endif()
endmacro()

MiddleWare_add(FreeRTOS)
MiddleWare_add(STAI)

# 过滤掉不应单独编译的模板文件（如 ll_aton_osal_rtos_template.c 通过 #include 被引用）
list(FILTER MiddleWare_SOURCES EXCLUDE REGEX "_template\\.c$")

# ============================================================
# 构建静态库
# ============================================================
add_library(MiddleWare_Lib STATIC ${MiddleWare_SOURCES})

target_include_directories(MiddleWare_Lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/include                           # FreeRTOS API 头文件
    ${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/portable/GCC/ARM_CM55_NTZ         # portmacro.h
)

target_include_directories(MiddleWare_Lib PRIVATE
    ${CMAKE_SOURCE_DIR}/Core/Inc                                           # FreeRTOSConfig.h
    ${CMAKE_SOURCE_DIR}/App/plam_search/AI
    ${CMAKE_SOURCE_DIR}/App/plam_search/Network
    ${CMAKE_CURRENT_SOURCE_DIR}/STAI/Npu  
    ${CMAKE_CURRENT_SOURCE_DIR}/STAI/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/STAI/Misc/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/STM32N6xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Core/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32N6xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Include
)

target_compile_definitions(MiddleWare_Lib PRIVATE ${COMMON_COMPILE_DEFINITIONS})
target_compile_definitions(MiddleWare_Lib PRIVATE
    LL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6
    LL_ATON_OSAL=LL_ATON_OSAL_FREERTOS
    LL_ATON_SW_FALLBACK=1
)
target_compile_options(MiddleWare_Lib PRIVATE ${COMMON_COMPILE_OPTIONS})

target_link_libraries(MiddleWare_Lib PUBLIC HAL_Lib)

# 链接 ST AI 预编译运行时库（包含 stai_get_return_code_name 等 API 实现）
# PUBLIC 以便传递到 App_Lib → Core 的链接链
target_link_libraries(MiddleWare_Lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/STAI/Lib/NetworkRuntime1200_CM55_GCC.a
)
