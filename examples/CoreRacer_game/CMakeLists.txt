# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Ferdinand ATI
# VecOs — Lightweight preemptive RTOS for 32-bit microcontrollers
# https://github.com/Ferdinaelectro1/VecOs

add_executable(CoreRacer_game main.cpp)

target_link_libraries(CoreRacer_game hardware_i2c vecos)

pico_add_extra_outputs(CoreRacer_game)