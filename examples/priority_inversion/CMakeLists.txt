# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Ferdinand ATI
# VecOs — Lightweight preemptive RTOS for 32-bit microcontrollers
# https://github.com/Ferdinaelectro1/VecOs

add_executable(priority_inversion main.cpp)

target_link_libraries(priority_inversion vecos)

pico_add_extra_outputs(priority_inversion)