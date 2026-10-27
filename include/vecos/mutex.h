// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VecOs — Lightweight preemptive RTOS for 32-bit microcontrollers
// https://github.com/Ferdinaelectro1/VecOs

#ifndef VECOS_MUTEX_H
#define VECOS_MUTEX_H

#include "vecos/tcb.hpp"
#include "vecos/utils.h"

namespace vecos {
    class Mutex {
        public:
          explicit Mutex(bool inheritance_enabled = true);
          void init_debug();

          //Lock mutex
          void lock();

          //Unlock mutex
          void unlock();

          //try to take lock , return true if success
          bool try_lock();

        private:
           TCB *_owner = nullptr;
           TCB *_head = nullptr;
           TCB *_tail = nullptr;
           bool _inheritance_enabled;
           volatile bool _locked;
    };
} //End vecos

#endif // VECOS_MUTEX_H