add_library(Libraries
    Src/gpio_lib.c
    Src/usart_lib.c
)

target_include_directories(Libraries PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

target_link_libraries(Libraries PUBLIC
    STM32_Drivers
)
