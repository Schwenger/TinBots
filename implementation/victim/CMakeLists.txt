cmake_minimum_required(VERSION 3.3)

set(CMAKE_SYSTEM_NAME "Generic")

project(Victim)

set(DEVICE "attiny2313")

set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_CXX_COMPILER avr-g++)

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

set(CMAKE_C_FLAGS  "-Os -mmcu=${DEVICE} -std=gnu99 -Wl,--gc-sections")
set(CMAKE_CXX_FLAGS "-Os -mmcu=${DEVICE} -std=c++14 -Wl,--gc-sections")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

include_directories("/usr/avr/include")
include_directories("../neatavr")

set(SOURCE_FILES main.cpp)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND avr-objcopy -O ihex -R.eeprom ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.hex)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom="alloc,load"  --change-section-lma .eeprom=0 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.eep)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND avr-size ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME} --mcu=${DEVICE} --format=avr)