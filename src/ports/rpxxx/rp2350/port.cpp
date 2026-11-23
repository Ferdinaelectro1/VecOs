// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VecOs — Lightweight preemptive RTOS for 32-bit microcontrollers
// https://github.com/Ferdinaelectro1/VecOs

#include "hardware/structs/scb.h"
#include "hardware/sync.h"
#include "pico/time.h"
#include "vecos/port.h"
#include "vecos/tcb.hpp"

namespace _asm {
    extern "C" {
        void PendSV_Handler(void);
    }

    extern "C" {
        void start_first_task(TCB **);
    }
}


#define TIMER_TICK 1

static bool repeating_timer_callback(struct repeating_timer *) {
    volatile uint32_t *icsr = (volatile uint32_t *)0xE000ED04;
    *icsr = 0x10000000;    
    return true;
}

struct repeating_timer timer;

void vecos::port::init_hardware_context() {
    add_repeating_timer_ms(-TIMER_TICK,repeating_timer_callback,NULL,&timer);

    uint32_t *vtor_table = (uint32_t *)scb_hw->vtor;

    vtor_table[14] = (uint32_t)_asm::PendSV_Handler;    

    volatile uint32_t *shpr3 = (volatile uint32_t *)0xE000ED20;

    *shpr3 = (*shpr3 & 0xFF00FFFF) | 0x00F00000;
}

void vecos::port::yield_cpu() {
    volatile uint32_t *icsr = (volatile uint32_t *)0xE000ED04;
    *icsr = 0x10000000;  
    __dsb(); 
    __isb();
}

void vecos::port::put_cpu_to_sleep() {
    __asm volatile("wfi");
}

uint32_t vecos::port::save_and_disable_interrupts()
{
    uint32_t result;
    __asm__ volatile (
        "mrs %0, primask \n"
        "cpsid i         \n"
        : "=l" (result)
        :
        : "memory"
    );
    return result;
}

void vecos::port::restore_interrupts(const uint32_t status)
{
    __asm__ volatile (
        "msr primask, %0 \n"
        :
        : "l" (status)
        : "memory"
    );
}

void vecos::port::task_init(void (*task_func_ptr)(void *), void *args, TCB *task_tcb)
{
    uint32_t *sp = task_tcb->stack_base;
    *sp-- = 0x01000000; 
    *sp-- = (uint32_t )task_func_ptr;
    *sp-- = 0;                        // LR
    *sp-- = 0;                        // R12
    *sp-- = 0;                        // R3
    *sp-- = 0;                        // R2
    *sp-- = 0;                        // R1
    *sp-- = (uint32_t)args;           // R0

    *sp-- = 0;                        // R11
    *sp-- = 0;                        // R10
    *sp-- = 0;                        // R9
    *sp-- = 0;                        // R8

    *sp-- = 0;                        // R7
    *sp-- = 0;                        // R6
    *sp-- = 0;                        // R5
    *sp   = 0;                        // R4
    task_tcb->stack_ptr = sp;
}

uint64_t vecos::port::get_time_us() {
    return time_us_64();
}

void vecos::port::start_first_task(TCB **) {

}