cmake_minimum_required(VERSION 3.3)

set(CMAKE_SYSTEM_NAME "Generic")

project(EPuck)

set(DEVICE "30F6014A")

set(CMAKE_FIND_ROOT_PATH /opt/microchip/xc16/v1.26)
set(CMAKE_C_COMPILER /opt/microchip/xc16/v1.26/bin/xc16-gcc)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

enable_language(ASM)

set(CMAKE_C_FLAGS "-mcpu=${DEVICE} -mlarge-code -mlarge-data -O0 -fmessage-length=0")
set(CMAKE_ASM_FLAGS ${CMAKE_C_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--heap=128,--stack=512,--report-mem,-Map=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.map,--script=/opt/microchip/xc16/v1.26/support/dsPIC30F/gld/p30F6014A.gld")

set(LIB_PATH "../../libraries/tinpuck")

include_directories(. hal lib src ${LIB_PATH}/include)

set(SOURCE_FILES
        main.c

        ${LIB_PATH}/src/adc.c
        ${LIB_PATH}/src/com.c
        ${LIB_PATH}/src/core.c
        ${LIB_PATH}/src/i2c.c
        ${LIB_PATH}/src/leds.c
        ${LIB_PATH}/src/motors.c
        ${LIB_PATH}/src/rs232.c
        ${LIB_PATH}/src/scheduler.c
        ${LIB_PATH}/src/selector.c
        ${LIB_PATH}/src/time.c
        ${LIB_PATH}/src/utils.c

        src/approximator.c
        src/astar/astar.c
        src/blind-cop.c
        src/controller.c
        src/path-exec.c
        src/path-finder.c
        src/rhr.c
        src/state-machine-common.c
        src/tinbot.c
        src/traffic-cop-eyes.c
        src/victim-direction.c
        src/victim-finder.c

        hal/hal_epuck.c)

add_executable(${CMAKE_PROJECT_NAME}.elf ${SOURCE_FILES})

add_custom_command(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD COMMAND /opt/microchip/xc16/v1.26/bin/xc16-bin2hex "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.elf")
