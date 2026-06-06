@echo off
setlocal

set "ROOT=C:\Users\shark\Desktop\N65_AI"
set "TOOLCHAIN=C:\Users\shark\AppData\Local\stm32cube\bundles\gnu-tools-for-stm32\14.3.1+st.2\bin"

set "CC=%TOOLCHAIN%\arm-none-eabi-gcc"
set "OBJCOPY=%TOOLCHAIN%\arm-none-eabi-objcopy"

set "SRC=%ROOT%\App\plam_search\Network\network_atonbuf.xSPI2.c"
set "OBJ=%ROOT%\tool\weights_temp.o"
set "BIN=%ROOT%\tool\weights.bin"

echo Compiling network_atonbuf.xSPI2.c ...

"%CC%" -mcpu=cortex-m55 -mthumb -c -O0 ^
  -I "%ROOT%\Middlewares\STAI\Npu" ^
  -I "%ROOT%\Middlewares\STAI\Inc" ^
  -I "%ROOT%\Drivers\CMSIS\Include" ^
  -I "%ROOT%\Drivers\CMSIS\Core\Include" ^
  -I "%ROOT%\Drivers\CMSIS\Device\ST\STM32N6xx\Include" ^
  -I "%ROOT%\Drivers\STM32N6xx_HAL_Driver\Inc" ^
  -I "%ROOT%\Core\Inc" ^
  -I "%ROOT%\Middlewares\FreeRTOS\include" ^
  -I "%ROOT%\Middlewares\FreeRTOS\portable\GCC\ARM_CM55_NTZ" ^
  -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 ^
  -DLL_ATON_OSAL=LL_ATON_OSAL_FREERTOS ^
  -DUSE_HAL_DRIVER ^
  -DSTM32N647xx ^
  -o "%OBJ%" "%SRC%"

if %errorlevel% neq 0 (
    echo COMPILE FAILED
    exit /b 1
)

echo Extracting .xspi2_weights section ...
"%OBJCOPY%" -O binary --only-section=.xspi2_weights "%OBJ%" "%BIN%"

if %errorlevel% neq 0 (
    echo OBJCOPY FAILED
    exit /b 1
)

for %%A in ("%BIN%") do set "SIZE=%%~zA"
echo.
echo ========================================
echo   SUCCESS
echo   Output: %BIN%
echo   Size:   %SIZE% bytes
echo   Target: 0x70200000 (XSPI2 NOR Flash)
echo ========================================

del "%OBJ%" 2>nul
endlocal
