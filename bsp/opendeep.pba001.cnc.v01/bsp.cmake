# Example of extension board specifics #

target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        PICO_DEFAULT_UART=0
        PICO_DEFAULT_UART_TX_PIN=0
        PICO_DEFAULT_UART_RX_PIN=1
        PICO_DEFAULT_UART_BAUD_RATE=115200
)

# Route stdio printfover the usb
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

