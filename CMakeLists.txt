cmake_minimum_required(VERSION 3.20)
include(pico_sdk_import.cmake)

project(bmp280_pio)

pico_sdk_init()

add_executable(bmp280_pio 
        bmp280_pio.c
        pio_spi.c)

pico_generate_pio_header(bmp280_pio ${CMAKE_CURRENT_LIST_DIR}/spi.pio)

target_link_libraries(bmp280_pio PRIVATE  
        pico_stdlib
        hardware_pio)


pico_enable_stdio_usb(bmp280_pio 1)
pico_enable_stdio_uart(bmp280_pio 0)

pico_add_extra_outputs(bmp280_pio)





