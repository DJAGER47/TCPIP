CMAKE_MINIMUM_REQUIRED (VERSION 3.10.0)

# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_TOOLCHAIN_FILE "armgcc.cmake" CACHE PATH "toolchain file")
SET(LD_FILE "STM32F746ZGTX_FLASH.ld")
ENABLE_LANGUAGE(C CXX ASM)

# CURRENT DIRECTORY
SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})
SET(EXECUTABLE_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})


# PROJECT INFOMATION
project( test_ETH )

set(EXECUTABLE ${PROJECT_NAME}.elf)

# Флаги для конкретного сборщика
include(${ProjDirPath}/flags.cmake)

# THE NEED BUILD SOURCE PATH AND BUILD ALL FILES
set ( SRC 
  ${ProjDirPath}/main.c
  ${ProjDirPath}/Src/startup_stm32f746xx.s
  ${ProjDirPath}/Src/stm32f7xx_it.c
  ${ProjDirPath}/Src/stm32f7xx_hal_timebase_tim.c
  ${ProjDirPath}/Src/system_stm32f7xx.c
  ${ProjDirPath}/lan8742/lan8742.c
  ${ProjDirPath}/Src/phy.c
  
  ${ProjDirPath}/Src/DefaultStack.cpp
  ${ProjDirPath}/Src/ethernetif.cpp
)
# GENERATE THE TARGET
add_executable (${EXECUTABLE}  ${SRC})

target_compile_definitions(${EXECUTABLE} PRIVATE
  -DUSE_HAL_DRIVER
  -DSTM32F746xx
)

# Общие флаги 
SET (SHARED_FLAGS -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mthumb)
target_compile_options(${EXECUTABLE} PRIVATE
  ${SHARED_FLAGS}
  -Wall
  -Wextra
  # -Werror
  -pedantic
  -Wconversion
  # -Wpadded
  -ffunction-sections # Каждая функция в отдельной секции в elf
  -fdata-sections     # Каждая переменная в отдельной секции в elf
  -fstack-usage       # Генерирует su файлы для анализа стека
)

target_link_options(${EXECUTABLE} PRIVATE 
  ${SHARED_FLAGS}
  -Wl,--start-group 
  -lm
  -lc
  -lgcc
  -lnosys
  -Wl,--end-group
  -Wl,--print-memory-usage
)


# SET INCLUDE PATH
set (INCLUDE
  ${ProjDirPath}/
  ${ProjDirPath}/Inc/
  ${ProjDirPath}/lan8742/
  ${ProjDirPath}/Drivers/CMSIS/Device/ST/STM32F7xx/Include
  ${ProjDirPath}/Drivers/CMSIS/Include
)
target_include_directories(${EXECUTABLE} PRIVATE ${INCLUDE})

set(CMAKE_MODULE_PATH
  ${ProjDirPath}/Drivers/STM32F7xx_HAL_Driver/
  ${ProjDirPath}/Drivers/FreeRTOS
  ${ProjDirPath}/RTT/
  ${ProjDirPath}/TCPIP/
)

# include modules
include(_hal_ll_f7_FR)
include(_debug)
include(_free_rtos)
include(_tcp_ip)


set_target_properties(${EXECUTABLE} PROPERTIES C_STANDARD   11)
set_target_properties(${EXECUTABLE} PROPERTIES CXX_STANDARD 17)

# POST BUILD
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD 
  COMMAND ${CMAKE_OBJCOPY} -Oihex   ${EXECUTABLE_OUTPUT_PATH}/${EXECUTABLE} ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.hex
  COMMAND ${CMAKE_OBJDUMP} -h -S    ${EXECUTABLE_OUTPUT_PATH}/${EXECUTABLE} > ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME}.list
  COMMAND ${CMAKE_SIZE_COMPILER} --format=berkeley ${EXECUTABLE_OUTPUT_PATH}/${EXECUTABLE}
)

# option(BUILD_TESTING "Build tests" ON)
# if(BUILD_TESTING)
#   enable_testing()
#   add_subdirectory( tests/ )
# endif()