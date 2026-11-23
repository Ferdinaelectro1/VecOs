# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Ferdinand ATI
# VecOs — Lightweight preemptive RTOS for 32-bit microcontrollers
# https://github.com/Ferdinaelectro1/VecOs

cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(vecos C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICOTOOL_FORCE_FETCH_FROM_GIT ON CACHE BOOL "" FORCE)

pico_sdk_init()

if (PICO_PLATFORM STREQUAL "rp2040") 
    message(STATUS "Building for RP2040 platform")
    set(PLATFORM_CODE  src/ports/rpxxx/rp2040/port.cpp
                   src/ports/rpxxx/common/init.S
                    src/ports/rpxxx/common/context_switch.S
    )
elseif (PICO_PLATFORM MATCHES "^rp2350")
    message(STATUS "Building for RP2350 platform")
    set(PLATFORM_CODE  src/ports/rpxxx/rp2350/port.cpp
                    src/ports/rpxxx/common/init.S
                    src/ports/rpxxx/common/context_switch.S
    )
else()
    message(FATAL_ERROR "Unsupported platform: ${PICO_PLATFORM}. Only RP2040 or rp2350 is supported.")
endif()

# ── La bibliothèque vecos ──────────────────────────────────────
add_library(vecos STATIC
    src/vecos/scheduler.cpp
    src/vecos/mutex.cpp
    src/vecos/semaphore.cpp
    ${PLATFORM_CODE}
)

target_include_directories(vecos PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(vecos PUBLIC
    pico_stdlib
    hardware_exception
)

#examples utility
add_subdirectory(examples/blink)
add_subdirectory(examples/mutex_and_print)
add_subdirectory(examples/producer_consumer)
add_subdirectory(examples/message_exchanger)
add_subdirectory(examples/CoreRacer_game)
add_subdirectory(examples/priority_task)
add_subdirectory(examples/semaphore_latency)
add_subdirectory(examples/stack_overflow_example)
add_subdirectory(examples/priority_inversion)
#end of examples utility