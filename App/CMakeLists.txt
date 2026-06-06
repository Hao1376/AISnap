# ============================================================
# App 应用层 → 静态库（App_Lib）
# 封装 AI 推理初始化、NPU 缓存管理、X-CUBE-AI 网络模型
# 依赖 MiddleWare_Lib（STAI runtime）和 HAL_Lib
# 链接到 Core/STM32N6_AI_Glasses.elf
# ============================================================

# ---- 源文件 ----
set(APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/plam_search/AI/app_x-cube-ai.c
    ${CMAKE_CURRENT_SOURCE_DIR}/plam_search/AI/npu_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/plam_search/AI/npu_cache.c
    ${CMAKE_CURRENT_SOURCE_DIR}/plam_search/AI/user_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/plam_search/AI/network_weights.c
    ${CMAKE_CURRENT_SOURCE_DIR}/plam_search/Network/network.c
    ${CMAKE_CURRENT_SOURCE_DIR}/plam_search/Network/stai_network.c
    ${CMAKE_CURRENT_SOURCE_DIR}/plam_search/Network/network_atonbuf.xSPI2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/plam_search/pd_anchors.c
)

# ---- 静态库 ----
add_library(App_Lib STATIC ${APP_SOURCES})

# ---- 包含路径 ----
# PUBLIC：传播到链接目标（Core）
# 注意：app_x-cube-ai.h（PUBLIC 头文件）包含了 stai.h、ll_aton_runtime.h 等 STAI 头文件，
#       因此 STAI 路径也必须 PUBLIC，否则 main.c #include "app_x-cube-ai.h" 会报错
target_include_directories(App_Lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/plam_search/AI                          # app_x-cube-ai.h, npu_cache.h, user_init.h
    ${CMAKE_CURRENT_SOURCE_DIR}/plam_search/Network                     # stai_network.h, network.h
    ${CMAKE_SOURCE_DIR}/Middlewares/STAI/Npu                # ll_aton_runtime.h（app_x-cube-ai.h:27 引用）
    ${CMAKE_SOURCE_DIR}/Middlewares/STAI/Inc                # stai.h（app_x-cube-ai.h:25 引用）
    ${CMAKE_SOURCE_DIR}/Middlewares/STAI/Misc/Inc           # STAI 杂项
)

# PRIVATE：仅 App_Lib 内部需要
target_include_directories(App_Lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/plam_search                   # pd_anchors.h
    ${CMAKE_SOURCE_DIR}/Core/Inc                            # main.h（app_x-cube-ai.h 间接引用）
    ${CMAKE_SOURCE_DIR}/Drivers/STM32N6xx_HAL_Driver/Inc    # HAL 驱动
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Core/Include          # CMSIS Core
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32N6xx/Include  # 设备头文件
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include               # CMSIS 通用
    ${CMAKE_SOURCE_DIR}/Middlewares
                         # FreeRTOS.h（间接）
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Include   # arm_math.h（pd_anchors.c 使用）
)

# ---- 编译宏 ----
target_compile_definitions(App_Lib PRIVATE ${COMMON_COMPILE_DEFINITIONS})
# PUBLIC：传播到 Core，因为 app_x-cube-ai.h（PUBLIC）→ ll_aton_runtime.h → ll_aton_config.h
# 需要 LL_ATON_PLATFORM 和 LL_ATON_OSAL 宏定义
target_compile_definitions(App_Lib PUBLIC
    LL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6
    LL_ATON_OSAL=LL_ATON_OSAL_FREERTOS
)
target_compile_options(App_Lib PRIVATE ${COMMON_COMPILE_OPTIONS})

# ---- 链接依赖 ----
target_link_libraries(App_Lib PUBLIC
    MiddleWare_Lib   # App_Lib 的用户（Core）自动获得 MiddleWare_Lib
    HAL_Lib          # npu_cache.c 调用 HAL_CACHEAXI API
)
