cmake_minimum_required(VERSION 3.19)

# Check [pico-sdk] and [picotool]
include(cmake/bringup_helper.cmake)

# Include the SDK import
include(pico_sdk_import.cmake)

# Project name
project(OpenDEEP)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Collect all font sources
file(GLOB FONT_SOURCES
        ${CMAKE_SOURCE_DIR}/src/static_fonts/*.cpp
)

# Add executables
add_executable(${PROJECT_NAME}
        app/main.cpp
        ext_boards/${EXT_BOARD}/bsp/bsp.hpp
        src/platform/platform.cpp
        src/lcd/graphics.cpp
        src/lcd/bitmaps.cpp
        src/lcd/lcd.cpp
        ${FONT_SOURCES}
        src/lcd/GC9107.cpp
        src/lcd/demo.cpp
        src/lcd/demo.hpp
)

# Include source files directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src/platform
        ${CMAKE_SOURCE_DIR}/src/lcd
        ${CMAKE_SOURCE_DIR}/src/static_fonts
        ext_boards/${EXT_BOARD}/bsp
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        pico_rand
        pico_stdlib
        hardware_pwm
        hardware_spi
        hardware_i2c
        hardware_dma
)

# Add floating-point printf support from newlib-nano
target_link_options(${PROJECT_NAME} PRIVATE "-Wl,-u,_printf_float")

# Create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})

# Add ELF size table to build report
include(cmake/pico_size_tool.cmake)

# Add extension board (if it specified)
if(DEFINED EXT_BOARD AND NOT EXT_BOARD STREQUAL "")
    message(STATUS "Using extension board [${EXT_BOARD}] specifics")
    include(ext_boards/${EXT_BOARD}/ext_board.cmake)
endif()
