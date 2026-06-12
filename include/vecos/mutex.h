// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VectOS — Lightweight preemptive RTOS for Microcontroller
// https://github.com/Ferdinaelectro1/VectOS

#ifndef VECOS_MUTEX_H
#define VECOS_MUTEX_H

#include "vecos/scheduler.h"

#define LED 27
#define LED2 28

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
           TCB *_waiting_mutex_tcb[16];
           uint16_t _waiting_mutex_tcb_count = 0;
           bool _locked;
    };
} //End vecos

#endif // VECOS_MUTEX_H