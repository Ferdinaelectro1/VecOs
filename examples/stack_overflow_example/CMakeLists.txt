# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Ferdinand ATI
# VecOs — Lightweight preemptive RTOS for 32-bit microcontrollers
# https://github.com/Ferdinaelectro1/VecOs

add_executable(stack_overflow_example main.cpp)

target_link_libraries(stack_overflow_example vecos)

pico_add_extra_outputs(stack_overflow_example)