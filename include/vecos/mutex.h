// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VectOS — Lightweight preemptive RTOS for Microcontroller
// https://github.com/Ferdinaelectro1/VectOS

#ifndef VECOS_MUTEX_H
#define VECOS_MUTEX_H

#include "vecos/tcb.hpp"
#include "vecos/utils.h"

namespace vecos {
    class Mutex {
        public:
          Mutex();
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
           volatile bool _locked;
    };
} //End vecos

#endif // VECOS_MUTEX_H