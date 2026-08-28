#
# Created by :: SPS :: on 27/08/2026.
#
# @Board: Virtual (host simulator)

set(SIM_DIR ${CMAKE_SOURCE_DIR}/bsp/${PRJ_BOARD}/sim)

target_sources(${PROJECT_NAME}
        PRIVATE
#        ${SIM_DIR}/src/sim_pico.cpp
#        ${SIM_DIR}/src/sim_panel.cpp
#        ${SIM_DIR}/src/sim_window.cpp
#        ${SIM_DIR}/src/sim_sensor.cpp
        # Shared with the unit-tests
        ${CMAKE_SOURCE_DIR}/tests/support/fake_ms5837_bus.cpp
)

# shadows pico/ and hardware/
target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${SIM_DIR}/include
        ${SIM_DIR}/src
        ${CMAKE_SOURCE_DIR}/tests/support
)

target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        PICO_DEFAULT_UART=0
        PICO_DEFAULT_UART_TX_PIN=0
        PICO_DEFAULT_UART_RX_PIN=1
        PICO_DEFAULT_UART_BAUD_RATE=115200
)

# SDL2
find_package(SDL2 QUIET)

if(NOT SDL2_FOUND)
    message(STATUS "[SIMULATOR] system SDL2 not found - fetching")
    include(FetchContent)
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL  "" FORCE)
    set(SDL_TEST OFF CACHE BOOL   "" FORCE)
    FetchContent_Declare(SDL2
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-2.30.9
            GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(SDL2)
else()
    message(STATUS "[SIMULATOR] using system SDL2")
endif()

# Target names handle
if(TARGET SDL2::SDL2-static)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2-static)
elseif(TARGET SDL2::SDL2)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
else()
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})
endif()

# Handle MinGW SDL2 linking
if(MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++ -static)
endif()
