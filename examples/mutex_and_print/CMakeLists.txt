# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Ferdinand ATI
# VecOs — Lightweight preemptive RTOS for 32-bit microcontrollers
# https://github.com/Ferdinaelectro1/VecOs

add_executable(mutex_and_print main.cpp)

target_link_libraries(mutex_and_print vecos)

pico_add_extra_outputs(mutex_and_print)