// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VecOs — Lightweight preemptive RTOS for 32-bit microcontrollers
// https://github.com/Ferdinaelectro1/VecOs

#pragma once
#include <stdint.h>

struct TCB;

namespace vecos {
    namespace port {

        // Initialise le timer système et injecte les interruptions de l'OS
        void init_hardware_context();

        // Force le CPU à abandonner la tâche courante (provoque un switch context)
        void yield_cpu();

        // Met le CPU au repos (basse consommation) dans la tâche Idle
        void put_cpu_to_sleep();

        //enregistre et désactive toutes les interruptions dans le cpu cible
        uint32_t save_and_disable_interrupts();
        
        //réactive les interruptions das le cpu cible
        void restore_interrupts(uint32_t status);

        //Init system stack
        void task_init(void (*task_func_ptr)(void *),void *args,TCB *task_tcb);

        //Starting first task
        void start_first_task(TCB **);

        uint64_t get_time_us();
    } // namespace port
} // namespace vecos