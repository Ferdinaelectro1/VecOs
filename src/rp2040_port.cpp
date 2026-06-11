// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VectOS — Lightweight preemptive RTOS for Microcontroller
// https://github.com/Ferdinaelectro1/VectOS

#include "hardware/structs/scb.h"
#include "pico/time.h"
#include "vecos/port.h"

#define TIMER_TICK 5

extern "C" {
    void SVC_Handler(void);
    void PendSV_Handler(void);
}

static bool repeating_timer_callback(struct repeating_timer *) {
    // Déclenche directement PendSV via l'ICSR (Interrupt Control and State Register)
    volatile uint32_t *icsr = (volatile uint32_t *)0xE000ED04;
    *icsr = 0x10000000; // Force le bit 28 (PendSV set-pending)    
    return true;
}

struct repeating_timer timer;

void vecos::port::init_hardware_context() {
    add_repeating_timer_ms(-TIMER_TICK,repeating_timer_callback,NULL,&timer);

    // --- INJECTION BRUTE DANS LA TABLE VTOR ---
    // On récupère l'adresse de la table des vecteurs actuellement utilisée en RAM
    uint32_t *vtor_table = (uint32_t *)scb_hw->vtor;
    
    // Case 11 = SVC (Supervisor Call)
    vtor_table[11] = (uint32_t)SVC_Handler;
    
    // Case 14 = PendSV (Pendable Service Call)
    vtor_table[14] = (uint32_t)PendSV_Handler;
    
    // Adresse du registre SHPR3 (System Handler Priority Register 3)
    volatile uint32_t *shpr3 = (volatile uint32_t *)0xE000ED20;

    // On force la priorité de PendSV (bits 16-23) à 0xC0 (priorité la plus basse sur M0+)
    *shpr3 = (*shpr3 & 0xFF00FFFF) | 0x00C00000;
}

void vecos::port::yield_cpu() {
    __asm volatile("svc #0");
}

void vecos::port::put_cpu_to_sleep() {
    __asm volatile("wfi");
}