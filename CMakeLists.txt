#CMake 最低版本号要求
cmake_minimum_required ( VERSION 3.1 )

#项目信息
project ( ROV )
set(CMAKE_SYSTEM_NAME Generic)
enable_language(ASM)

#STDLIB
add_subdirectory ( Source/STM32StdLib )
set (CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)

#包含的头文件
include_directories ( Source/STM32StdLib/Include/CMSIS/Device )
include_directories ( Source/STM32StdLib/Include/CMSIS )
include_directories ( Source/STM32StdLib/Include/STM32PeripheralsDriver)
include_directories ( Source/STM32StdLib/Include )
include_directories ( Source/Include )

#需要编译的文件
aux_source_directory ( Source/ DIR_MAIN_SRCS )
aux_source_directory ( Source/STM32StdLib/Source/CMSIS/ DIR_CMSIS_SRCS )

#CC AR LD AS
set ( CMAKE_C_COMPILER "arm-none-eabi-gcc" )
set ( CMAKE_C_AR "arm-none-eabi-ar" )
set ( CMAKE_ASM_COMPILER "/usr/bin/arm-none-eabi-gcc" )

#CFLAGS
set ( CMAKE_C_FLAGS "-g -Os -Wall -nostartfiles -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16")
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--gc-sections -u _printf_nano" )
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=hard -mabi=aapcs --specs=rdimon.specs" )
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--start-group -lgcc -lc -lm -lrdimon -Wl,--end-group" )
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D USE_STDPERIPH_DRIVER -D STM32F40_41xxx -D HSE_VALUE_25M" )

#LDFLAGS
set ( CMAKE_EXE_LINKER_FLAGS "-nostartfiles -Wl,--gc-sections -mthumb -mcpu=cortex-m4" CACHE INTERNAL "exe link flags")
set ( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ldscript/stm32f4_flash.ld" )

#ASMFLAGS
set ( CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}" CACHE INTERNAL "asm compiler flags" )

#指定生成目标
add_executable ( main.elf ${DIR_MAIN_SRCS} ${DIR_CMSIS_SRCS} Source/STM32StdLib/Source/CMSIS/startup_stm32f40_41xxx.s ) 

#链接静态库
target_link_libraries ( main.elf STM32F4StdPeripherals )

#生成二进制文件
add_custom_target ( main.bin DEPENDS main.elf COMMAND arm-none-eabi-objcopy -Obinary main.elf main.bin )
add_custom_target ( size DEPENDS main.elf COMMAND arm-none-eabi-size main.elf )


