# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Ferdinand ATI
# VecOs — Lightweight preemptive RTOS for 32-bit microcontrollers
# https://github.com/Ferdinaelectro1/VecOs

add_executable(semaphore_latency main.cpp)

target_link_libraries(semaphore_latency vecos)

pico_add_extra_outputs(semaphore_latency)