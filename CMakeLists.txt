cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(vecos C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICOTOOL_FORCE_FETCH_FROM_GIT ON CACHE BOOL "" FORCE)

pico_sdk_init()

# ── La bibliothèque vecos ──────────────────────────────────────
add_library(vecos STATIC
    src/vecos/scheduler.cpp
    src/vecos/mutex.cpp
    src/vecos/semaphore.cpp
    src/arch/rp2040_port.cpp
    src/ports/rp2040_init.S
    src/ports/rp2040_context_switch.S
)

target_include_directories(vecos PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vecos PUBLIC
    pico_stdlib
    hardware_exception
)

add_subdirectory(examples/blink)
add_subdirectory(examples/mutex_and_print)