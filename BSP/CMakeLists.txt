# ============================================================
# BSP 外设驱动 → 静态库（BSP_Lib）
# 每个外设独立开关，按需启用 ON/OFF
# 依赖 HAL_Lib（BSP 驱动调用 HAL API）
# ============================================================

# ============================================================
# AI 眼镜核心外设（默认 ON）
# ============================================================
set(BSP_OV5640    ON CACHE BOOL "OV5640 摄像头")       # 手势识别/跌倒检测输入
set(BSP_HYPERRAM  ON  CACHE BOOL "HyperRAM 外部内存")    # 帧缓冲 / NPU 工作区
set(BSP_NORFLASH  ON CACHE BOOL "NOR Flash (XSPI)")     # AI 模型权重存储
set(BSP_UART      ON CACHE BOOL "UART 调试串口")        # 调试日志输出
set(BSP_SYS       ON  CACHE BOOL "系统初始化/延时")       # delay_ms / 时钟配置

# ============================================================
# 音频 / 显示 / 输入（AI 眼镜可能需要，默认 OFF）
# ============================================================
set(BSP_ES8388    OFF CACHE BOOL "ES8388 音频 Codec")   # TTS 语音输出
set(BSP_LCD       OFF CACHE BOOL "LCD 显示屏")           # UI 显示
set(BSP_RGBLCD    ON  CACHE BOOL "RGB LCD 显示屏")
set(BSP_OLED      OFF CACHE BOOL "OLED 小屏")
set(BSP_LED       ON  CACHE BOOL "LED 指示灯")           # 状态指示
set(BSP_KEY       OFF CACHE BOOL "按键输入")             # 用户按键
set(BSP_TOUCH     OFF CACHE BOOL "触摸屏")               # 触摸输入
set(BSP_RNG       OFF CACHE BOOL "硬件随机数生成器")     # 加密/随机数

# ============================================================
# 可选传感器（默认 OFF）
# ============================================================
set(BSP_AP3216C   OFF CACHE BOOL "AP3216C 光感/接近")
set(BSP_DHT11     OFF CACHE BOOL "DHT11 温湿度")
set(BSP_DS18B20   OFF CACHE BOOL "DS18B20 温度")
set(BSP_QMC6308   OFF CACHE BOOL "QMC6308 地磁")
set(BSP_QMI8658A  OFF CACHE BOOL "QMI8658A IMU")

# ============================================================
# 定时器 / PWM / 捕获（默认 OFF）
# ============================================================
set(BSP_ATIM      OFF CACHE BOOL "高级定时器")
set(BSP_BTIM      OFF CACHE BOOL "基本定时器")
set(BSP_GTIM      OFF CACHE BOOL "通用定时器")
set(BSP_PWMDAC    OFF CACHE BOOL "PWM DAC")
set(BSP_IWDG      OFF CACHE BOOL "独立看门狗")
set(BSP_WWDG      OFF CACHE BOOL "窗口看门狗")

# ============================================================
# 通信接口（默认 OFF）
# ============================================================
set(BSP_FDCAN     OFF CACHE BOOL "FDCAN 总线")
set(BSP_RS485     OFF CACHE BOOL "RS485 总线")
set(BSP_NRF24L01  OFF CACHE BOOL "NRF24L01 无线")
set(BSP_REMOTE    OFF CACHE BOOL "红外遥控")
set(BSP_SPDIFRX   OFF CACHE BOOL "SPDIF 音频输入")

# ============================================================
# 存储（默认 OFF）
# ============================================================
set(BSP_SD_CARD   OFF CACHE BOOL "SD 卡")
set(BSP_SD_NAND   OFF CACHE BOOL "SD NAND")
set(BSP_EEPROM    OFF CACHE BOOL "EEPROM (I2C)")

# ============================================================
# 其他外设（默认 OFF）
# ============================================================
set(BSP_ADC       OFF CACHE BOOL "ADC 模数转换")
set(BSP_BEEP      OFF CACHE BOOL "蜂鸣器")
set(BSP_DTS       OFF CACHE BOOL "数字温度传感器")
set(BSP_EXTI      OFF CACHE BOOL "外部中断")
set(BSP_IMX335    OFF CACHE BOOL "IMX335 摄像头")
set(BSP_JPEGCODEC OFF CACHE BOOL "JPEG 硬件编解码")
set(BSP_RTC       OFF CACHE BOOL "RTC 实时时钟")
set(BSP_TPAD      OFF CACHE BOOL "触摸按键")
set(BSP_WS2812B   OFF CACHE BOOL "WS2812B RGB 灯带 (SPI)")
set(BSP_WS2812B_IO OFF CACHE BOOL "WS2812B RGB 灯带 (IO)")
set(BSP_YT8512C   OFF CACHE BOOL "YT8512C 以太网 PHY")

# ============================================================
# 收集已启用的源文件
# ============================================================
set(BSP_SOURCES
    bsp_init.c   # Unified BSP init (LED, LCD, UART, OV5640, DCMIPP pipeline)
)

macro(bsp_add COMPONENT)
    if(BSP_${COMPONENT})
        file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${COMPONENT}/*.c)
        list(APPEND BSP_SOURCES ${SRC_FILES})
        message(STATUS "  BSP: ${COMPONENT} (ON)")
    endif()
endmacro()

bsp_add(ADC)
bsp_add(AP3216C)
bsp_add(ATIM)
bsp_add(BEEP)
bsp_add(BTIM)
bsp_add(DHT11)
bsp_add(DS18B20)
bsp_add(DTS)
bsp_add(EEPROM)
bsp_add(ES8388)
bsp_add(EXTI)
bsp_add(FDCAN)
bsp_add(GTIM)
bsp_add(HYPERRAM)
bsp_add(IMX335)
bsp_add(IWDG)
bsp_add(JPEGCODEC)
bsp_add(KEY)
bsp_add(LCD)
bsp_add(LED)
bsp_add(NORFLASH)
bsp_add(NRF24L01)
bsp_add(OLED)
bsp_add(OV5640)
bsp_add(PWMDAC)
bsp_add(QMC6308)
bsp_add(QMI8658A)
bsp_add(REMOTE)
bsp_add(RGBLCD)
bsp_add(RNG)
bsp_add(RS485)
bsp_add(RTC)
bsp_add(SD_CARD)
bsp_add(SD_NAND)
bsp_add(SPDIFRX)
bsp_add(SYS)
bsp_add(TOUCH)
bsp_add(TPAD)
bsp_add(UART)
bsp_add(WS2812B)
bsp_add(WS2812B_IO)
bsp_add(WWDG)
bsp_add(YT8512C)

# ============================================================
# 构建静态库
# ============================================================
add_library(BSP_Lib STATIC ${BSP_SOURCES})

target_include_directories(BSP_Lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(BSP_Lib PRIVATE
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/App/plam_search/AI                          # app_x-cube-ai.h（bsp_init.c 使用）
    ${CMAKE_SOURCE_DIR}/App/plam_search/Network                     # stai_network.h（app_x-cube-ai.h 间接引用）
    ${CMAKE_SOURCE_DIR}/Middlewares/STAI/Inc                        # stai.h（app_x-cube-ai.h 间接引用）
    ${CMAKE_SOURCE_DIR}/Middlewares/STAI/Npu                        # ll_aton_runtime.h → ll_aton_osal_freertos.h
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/include                # FreeRTOS.h（ll_aton_osal_freertos.h 间接引用）
    ${CMAKE_SOURCE_DIR}/Middlewares/FreeRTOS/portable/GCC/ARM_CM55_NTZ  # portmacro.h（FreeRTOS.h 间接引用）
    ${CMAKE_SOURCE_DIR}/Drivers/STM32N6xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Core/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32N6xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Include
)

target_compile_definitions(BSP_Lib PRIVATE ${COMMON_COMPILE_DEFINITIONS})
target_compile_definitions(BSP_Lib PRIVATE
    LL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6
    LL_ATON_OSAL=LL_ATON_OSAL_FREERTOS
)
target_compile_options(BSP_Lib PRIVATE ${COMMON_COMPILE_OPTIONS})

target_link_libraries(BSP_Lib PUBLIC HAL_Lib)
