// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VectOS — Lightweight preemptive RTOS for Microcontroller
// https://github.com/Ferdinaelectro1/VectOS

#pragma once
#include <stdint.h>

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

    } // namespace port
} // namespace vecos