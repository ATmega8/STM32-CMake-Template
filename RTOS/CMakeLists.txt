#CMake 最低版本号要求 
cmake_minimum_required (VERSION 2.8)

#项目名称
project (FreeRTOS)

#源码文件夹
aux_source_directory (src SRCS_DIR)

include_directories (inc)

#CC AR
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_C_AR "arm-none-eabi-ar")

#CFLAGS
set (CMAKE_C_FLAGS "-Wall")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostartfiles -Wl,--gc-sections -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 ")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS}-mfloat-abi=hard -mabi=aapcs")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D USE_STDPERIPH_DRIVER -D STM32F40_41xxx")

#生成静态链接库
add_library (FreeRTOS ${SRCS_DIR})

