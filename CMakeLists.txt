cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)


project(pico_gpu_v1 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(pico_gpu_v1
	main.cpp
	)

target_link_libraries(pico_gpu_v1 pico_stdlib)

pico_enable_stdio_usb(pico_gpu_v1 1)
pico_enable_stdio_uart(pico_gpu_v1 0)

pico_add_extra_outputs(pico_gpu_v1)

