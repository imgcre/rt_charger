CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

SET(CMAKE_SYSTEM_NAME Generic)
#SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(CMAKE_C_COMPILER "/usr/lib/gcc/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-gcc")
SET(CMAKE_CXX_COMPILER "/usr/lib/gcc/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-g++")
SET(CMAKE_ASM_COMPILER "/usr/lib/gcc/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-gcc")
SET(CMAKE_OBJCOPY "/usr/lib/gcc/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-objcopy")
SET(CMAKE_SIZE "/usr/lib/gcc/gcc-arm-none-eabi-10-2020-q4-major/bin/arm-none-eabi-size")

SET(CMAKE_C_FLAGS " -mcpu=cortex-m3 -mthumb -ffunction-sections -fdata-sections -Dgcc -DHAVE_SIGVAL -DHAVE_SIGEVENT -DHAVE_SIGINFO -O2 -gdwarf-2 -g")
SET(CMAKE_CXX_FLAGS " -mcpu=cortex-m3 -mthumb -ffunction-sections -fdata-sections -Dgcc -DHAVE_SIGVAL -DHAVE_SIGEVENT -DHAVE_SIGINFO -O2 -gdwarf-2 -g -std=gnu++20")
SET(CMAKE_ASM_FLAGS " -c -mcpu=cortex-m3 -mthumb -ffunction-sections -fdata-sections -x assembler-with-cpp -Wa,-mimplicit-it=thumb  -gdwarf-2")
SET(CMAKE_EXE_LINKER_FLAGS " -mcpu=cortex-m3 -mthumb -ffunction-sections -fdata-sections -Wl,--gc-sections,-Map=rt-thread.map,-cref,-u,Reset_Handler -T ${CMAKE_SOURCE_DIR}/board/linker_scripts/link.lds")

SET(CMAKE_CXX_STANDARD 20)
PROJECT(rtthread C CXX ASM)
INCLUDE_DIRECTORIES(
	/root/projects/stm32f10x
	/root/projects/stm32f10x/applications
	/root/projects/stm32f10x/board
	/root/projects/stm32f10x/board/CubeMX_Config/Inc
	/root/projects/stm32f10x/configs
	/root/projects/stm32f10x/devices
	/root/projects/stm32f10x/hal
	/root/projects/stm32f10x/libraries/HAL_Drivers
	/root/projects/stm32f10x/libraries/HAL_Drivers/config
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/CMSIS/Device/ST/STM32F1xx/Include
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/CMSIS/Include
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/STM32F1xx_HAL_Driver/Inc
	/root/projects/stm32f10x/os
	/root/projects/stm32f10x/rt-thread/components/cplusplus
	/root/projects/stm32f10x/rt-thread/components/drivers/include
	/root/projects/stm32f10x/rt-thread/components/drivers/spi
	/root/projects/stm32f10x/rt-thread/components/drivers/spi/sfud/inc
	/root/projects/stm32f10x/rt-thread/components/finsh
	/root/projects/stm32f10x/rt-thread/components/libc/compilers/common
	/root/projects/stm32f10x/rt-thread/components/libc/compilers/newlib
	/root/projects/stm32f10x/rt-thread/include
	/root/projects/stm32f10x/rt-thread/libcpu/arm/common
	/root/projects/stm32f10x/rt-thread/libcpu/arm/cortex-m3
	/root/projects/stm32f10x/termios
	/root/projects/stm32f10x/tests
	/root/projects/stm32f10x/ui
	/root/projects/stm32f10x/utilities
)

ADD_DEFINITIONS(
	-DHAVE_CCONFIG_H
	-DRT_USING_NEWLIB
	-DSTM32F103xB
	-DUSE_HAL_DRIVER
	-D__RTTHREAD__
)

SET(PROJECT_SOURCES
	/root/projects/stm32f10x/applications/main.c
	/root/projects/stm32f10x/applications/test.cxx
	/root/projects/stm32f10x/configs/bsp.cxx
	/root/projects/stm32f10x/configs/os.cxx
	/root/projects/stm32f10x/rt-thread/components/cplusplus/cxx_Thread.cpp
	/root/projects/stm32f10x/rt-thread/components/cplusplus/cxx_crt.cpp
	/root/projects/stm32f10x/rt-thread/components/cplusplus/cxx_Mutex.cpp
	/root/projects/stm32f10x/rt-thread/components/cplusplus/cxx_Semaphore.cpp
	/root/projects/stm32f10x/rt-thread/components/cplusplus/crt_init.c
	/root/projects/stm32f10x/rt-thread/libcpu/arm/common/div0.c
	/root/projects/stm32f10x/rt-thread/libcpu/arm/common/backtrace.c
	/root/projects/stm32f10x/rt-thread/libcpu/arm/common/showmem.c
	/root/projects/stm32f10x/rt-thread/libcpu/arm/cortex-m3/cpuport.c
	/root/projects/stm32f10x/rt-thread/libcpu/arm/cortex-m3/context_gcc.S
	/root/projects/stm32f10x/rt-thread/components/drivers/misc/pin.c
	/root/projects/stm32f10x/rt-thread/components/drivers/serial/serial.c
	/root/projects/stm32f10x/rt-thread/components/drivers/spi/spi_core.c
	/root/projects/stm32f10x/rt-thread/components/drivers/spi/sfud/src/sfud.c
	/root/projects/stm32f10x/rt-thread/components/drivers/spi/spi_flash_sfud.c
	/root/projects/stm32f10x/rt-thread/components/drivers/spi/spi_dev.c
	/root/projects/stm32f10x/rt-thread/components/drivers/spi/sfud/src/sfud_sfdp.c
	/root/projects/stm32f10x/rt-thread/components/drivers/src/pipe.c
	/root/projects/stm32f10x/rt-thread/components/drivers/src/waitqueue.c
	/root/projects/stm32f10x/rt-thread/components/drivers/src/ringblk_buf.c
	/root/projects/stm32f10x/rt-thread/components/drivers/src/workqueue.c
	/root/projects/stm32f10x/rt-thread/components/drivers/src/dataqueue.c
	/root/projects/stm32f10x/rt-thread/components/drivers/src/ringbuffer.c
	/root/projects/stm32f10x/rt-thread/components/drivers/src/completion.c
	/root/projects/stm32f10x/devices/relay.cxx
	/root/projects/stm32f10x/devices/ws2812.cxx
	/root/projects/stm32f10x/devices/load_detector.cxx
	/root/projects/stm32f10x/devices/wtn6.cxx
	/root/projects/stm32f10x/devices/output_pin.cxx
	/root/projects/stm32f10x/devices/rc522.cxx
	/root/projects/stm32f10x/board/board.c
	/root/projects/stm32f10x/board/CubeMX_Config/Src/stm32f1xx_hal_msp.c
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xb.s
	/root/projects/stm32f10x/libraries/HAL_Drivers/drv_usart.c
	/root/projects/stm32f10x/libraries/HAL_Drivers/drv_spi.c
	/root/projects/stm32f10x/libraries/HAL_Drivers/drv_common.c
	/root/projects/stm32f10x/libraries/HAL_Drivers/drv_gpio.c
	/root/projects/stm32f10x/rt-thread/components/finsh/cmd.c
	/root/projects/stm32f10x/rt-thread/components/finsh/msh.c
	/root/projects/stm32f10x/rt-thread/components/finsh/shell.c
	/root/projects/stm32f10x/hal/raw_port.cxx
	/root/projects/stm32f10x/hal/port.cxx
	/root/projects/stm32f10x/hal/abs_port.cxx
	/root/projects/stm32f10x/rt-thread/src/device.c
	/root/projects/stm32f10x/rt-thread/src/scheduler.c
	/root/projects/stm32f10x/rt-thread/src/object.c
	/root/projects/stm32f10x/rt-thread/src/thread.c
	/root/projects/stm32f10x/rt-thread/src/components.c
	/root/projects/stm32f10x/rt-thread/src/kservice.c
	/root/projects/stm32f10x/rt-thread/src/idle.c
	/root/projects/stm32f10x/rt-thread/src/mempool.c
	/root/projects/stm32f10x/rt-thread/src/ipc.c
	/root/projects/stm32f10x/rt-thread/src/timer.c
	/root/projects/stm32f10x/rt-thread/src/mem.c
	/root/projects/stm32f10x/rt-thread/src/clock.c
	/root/projects/stm32f10x/rt-thread/src/irq.c
	/root/projects/stm32f10x/rt-thread/components/libc/compilers/common/time.c
	/root/projects/stm32f10x/rt-thread/components/libc/compilers/common/stdlib.c
	/root/projects/stm32f10x/rt-thread/components/libc/compilers/newlib/syscalls.c
	/root/projects/stm32f10x/rt-thread/components/libc/compilers/newlib/stdio.c
	/root/projects/stm32f10x/rt-thread/components/libc/compilers/newlib/libc.c
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_crc.c
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_spi.c
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cec.c
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_usart.c
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c
	/root/projects/stm32f10x/libraries/STM32F1xx_HAL/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
	/root/projects/stm32f10x/os/timer.cxx
	/root/projects/stm32f10x/os/deliver_base.cxx
	/root/projects/stm32f10x/os/thread.cxx
	/root/projects/stm32f10x/os/post.cxx
	/root/projects/stm32f10x/os/task.cxx
	/root/projects/stm32f10x/os/event.cxx
	/root/projects/stm32f10x/termios/cmd.cxx
	/root/projects/stm32f10x/tests/devices.rc522.cxx
	/root/projects/stm32f10x/ui/colors.cxx
)

LINK_DIRECTORIES(
)

LINK_LIBRARIES(
	c
	m
)

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME}.elf ${PROJECT_SOURCES})
ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD 
COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin COMMAND ${CMAKE_SIZE} ${CMAKE_PROJECT_NAME}.elf)