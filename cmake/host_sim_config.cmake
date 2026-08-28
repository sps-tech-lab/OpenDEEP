#
# Created by :: SPS :: on 27/08/2026.
#
# @Board: Virtual (host simulator)

macro(config_host_sim)

    if(NOT DEFINED PRJ_BOARD OR PRJ_BOARD STREQUAL "")
        message(FATAL_ERROR "PRJ_HOST_SIM needs PRJ_BOARD to name a simulator board")
    endif()

    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/bsp/${PRJ_BOARD}/bsp.cmake")
        message(FATAL_ERROR "No BSP found at bsp/${PRJ_BOARD}")
    endif()

    message(STATUS "Configure host simulator [${PRJ_BOARD}]")
    project(OpenDEEP LANGUAGES C CXX)

    # Generate compile_commands.json
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # Enable clang-tools like targets
    include(cmake/clang_tools.cmake)
    rp2_enable_clang_tools()

    # Collect all fonts
    file(GLOB FONT_SOURCES
            ${CMAKE_SOURCE_DIR}/src/fonts/*.cpp
    )

    add_executable(${PROJECT_NAME}
            app/main.cpp
            bsp/${PRJ_BOARD}/bsp/bsp.cpp
            bsp/${PRJ_BOARD}/bsp/bsp.hpp
            src/platform/platform.cpp
            src/lcd/graphics.cpp
            src/lcd/bitmaps.cpp
            src/lcd/lcd.cpp
            ${FONT_SOURCES}
            src/lcd/GC9107.cpp
            src/lcd/lcd_check.cpp
            src/lcd/lcd_check.hpp
            src/MS5837/MS5837.cpp
            src/MS5837/MS5837.hpp
            src/i2c_tools/i2c_tools.cpp
            src/i2c_tools/i2c_tools.hpp
    )

    target_include_directories(${PROJECT_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/src/platform
            ${CMAKE_SOURCE_DIR}/src/lcd
            ${CMAKE_SOURCE_DIR}/src/fonts
            ${CMAKE_SOURCE_DIR}/src/MS5837
            ${CMAKE_SOURCE_DIR}/src/i2c_tools
            bsp/${PRJ_BOARD}/bsp
    )

    target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

    include(bsp/${PRJ_BOARD}/bsp.cmake)

endmacro()
