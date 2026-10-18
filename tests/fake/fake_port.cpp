// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VectOS — Lightweight preemptive RTOS for Microcontroller
// https://github.com/Ferdinaelectro1/VectOS

#include "../../include/vecos/port.h"
#include "../../include/vecos/tcb.hpp"

#define TIMER_TICK 1

uint64_t g_fake_time_us = 0;

extern "C" {
    void PendSV_Handler(void) {

    }
}

extern "C" {
    void start_first_task(TCB **) {

    }
};

void vecos::port::init_hardware_context() {
}

void vecos::port::yield_cpu() {
}

void vecos::port::put_cpu_to_sleep() {
}

uint32_t vecos::port::save_and_disable_interrupts()
{
    return 0;
}

void vecos::port::restore_interrupts(const uint32_t status)
{
}

void vecos::port::task_init(void (*task_func_ptr)(void *), void *args, TCB *task_tcb)
{
}

uint64_t vecos::port::get_time_us() {
    return g_fake_time_us;
}