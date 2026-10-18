// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VecOs — Lightweight preemptive RTOS for 32-bit microcontrollers
// https://github.com/Ferdinaelectro1/VecOs

#pragma once
#include <stdint.h>
#include "vecos/tcb.hpp"

namespace vecos {
    class Semaphore {
        public:
           Semaphore(uint32_t initial_count = 1);
           void wait();
           void signal();

        private:
           uint32_t _counter;
           TCB *_waiting_tcb;

    };
} //End vecos
