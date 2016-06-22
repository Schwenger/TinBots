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

set(LIB_PATH "../../libraries/e-puck/library")

include_directories(. hal src)

include_directories(
        ${LIB_PATH}
        ${LIB_PATH}/uart
        ${LIB_PATH}/motor_led
        ${LIB_PATH}/bluetooth)

set(SOURCE_FILES
        main.c
        e_time.c

        src/tinbot.c
        src/rhr.c

        hal/hal_epuck.c

        ${LIB_PATH}/a_d/advance_ad_scan/e_ad_conv.c
        ${LIB_PATH}/a_d/advance_ad_scan/e_acc.c
        ${LIB_PATH}/a_d/advance_ad_scan/e_prox.c

        ${LIB_PATH}/acc_gyro/e_lsm330.c

        ${LIB_PATH}/I2C/e_I2C_master_module.c
        ${LIB_PATH}/I2C/e_I2C_protocol.c

        ${LIB_PATH}/uart/e_init_uart1.s
        ${LIB_PATH}/uart/e_init_uart2.s
        ${LIB_PATH}/uart/e_uart1_rx_char.s
        ${LIB_PATH}/uart/e_uart1_tx_char.s
        ${LIB_PATH}/uart/e_uart2_rx_char.s
        ${LIB_PATH}/uart/e_uart2_tx_char.s

        ${LIB_PATH}/bluetooth/e_bluetooth.c

        ${LIB_PATH}/motor_led/e_init_port.c

        ${LIB_PATH}/motor_led/advance_one_timer/e_motors.c
        ${LIB_PATH}/motor_led/advance_one_timer/e_agenda.c
        ${LIB_PATH}/motor_led/advance_one_timer/e_led.c

        ${LIB_PATH}/utility/utility.c)

add_executable(${CMAKE_PROJECT_NAME}.elf ${SOURCE_FILES})

add_custom_command(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD COMMAND /opt/microchip/xc16/v1.26/bin/xc16-bin2hex "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.elf")
