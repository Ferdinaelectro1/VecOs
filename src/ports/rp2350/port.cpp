// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VecOs — Lightweight preemptive RTOS for 32-bit microcontrollers
// https://github.com/Ferdinaelectro1/VecOs

#include "vecos/port.h"
#include "vecos/tcb.hpp"

#define TIMER_TICK 1

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
    return 0;
}

void vecos::port::start_first_task(TCB **) {

}