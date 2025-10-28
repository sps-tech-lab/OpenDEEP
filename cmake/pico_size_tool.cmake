find_program(ELF_SIZE_EXECUTABLE
        NAMES
        arm-none-eabi-size      # GCC ARM toolchain (Win/Linux/macOS)
        gsize                   # Homebrew binutils on macOS
        size                    # System 'size' (BSD on macOS, GNU on Linux)
        HINTS
        ENV PATH
)
if (NOT ELF_SIZE_EXECUTABLE)
    message(WARNING "Could not find 'size' in your PATH. Size summary won't be available!")
    return()
else()
    message(STATUS "Using size tool: ${ELF_SIZE_EXECUTABLE}")

    set(SIZE_FORMAT_ARG "")

    execute_process(
            COMMAND ${ELF_SIZE_EXECUTABLE} --version
            OUTPUT_VARIABLE _SIZE_VER
            ERROR_VARIABLE  _SIZE_VER_ERR
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE _SIZE_VER_RC
    )

    set(_SIZE_IS_GNU FALSE)
    if (_SIZE_VER_RC EQUAL 0)
        string(TOLOWER "${_SIZE_VER}${_SIZE_VER_ERR}" _SIZE_VER_ALL)
        if (_SIZE_VER_ALL MATCHES "gnu" OR _SIZE_VER_ALL MATCHES "arm-none-eabi")
            set(_SIZE_IS_GNU TRUE)
        endif()
    else()
        # '--version' not supported, check '--help'
        execute_process(
                COMMAND ${ELF_SIZE_EXECUTABLE} --help
                OUTPUT_VARIABLE _SIZE_HELP
                ERROR_VARIABLE  _SIZE_HELP_ERR
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_STRIP_TRAILING_WHITESPACE
                RESULT_VARIABLE _SIZE_HELP_RC
        )
        if (_SIZE_HELP_RC EQUAL 0)
            string(TOLOWER "${_SIZE_HELP}${_SIZE_HELP_ERR}" _SIZE_HELP_ALL)
            if (_SIZE_HELP_ALL MATCHES "--format=gnu")
                set(_SIZE_IS_GNU TRUE)
            endif()
        endif()
    endif()

    if (_SIZE_IS_GNU)
        set(SIZE_FORMAT_ARG "--format=gnu")   # GNU/ARM size
    else()
        set(SIZE_FORMAT_ARG "")               # BSD/macOS size
    endif()

    find_program(SIZE_SUMMARY_SCRIPT
            NAMES pico_size_tool.py
            HINTS "${CMAKE_SOURCE_DIR}/scripts"
    )
    if (NOT SIZE_SUMMARY_SCRIPT)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${ELF_SIZE_EXECUTABLE} --format=gnu $<TARGET_FILE:${PROJECT_NAME}>
                VERBATIM
        )
    else()
        message(STATUS "Using user-friendly size report")
        math(EXPR FLASH_SIZE_EVAL "${PICO_FLASH_SIZE_BYTES}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND python3 ${SIZE_SUMMARY_SCRIPT}
                --flash-size ${FLASH_SIZE_EVAL}
                --platform ${PICO_PLATFORM}
                $<TARGET_FILE:${PROJECT_NAME}>
        )
    endif()
endif()