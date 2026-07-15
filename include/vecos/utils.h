// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VectOS — Lightweight preemptive RTOS for RP2040
// https://github.com/Ferdinaelectro1/VectOS

#pragma once
#include <stdint.h>

#include "vecos/tcb.hpp"

static constexpr uint16_t HARD_MAX_TASK = 16;
namespace vecos {
    namespace utils
    {
        inline void append(TCB*& head, TCB*& tail, TCB *task) {
            if(task == nullptr) return;
            task->next_blocked = nullptr;
            if(head) {
                tail->next_blocked = task;
                tail = tail->next_blocked;
            } else {
                head = task;
                tail = task;
            }
        }

        inline  TCB* read_front(TCB*& head, TCB*& tail) {
            if(head == nullptr) return nullptr;
            TCB *read_tcb = head;
            head = head->next_blocked;
            if(head == nullptr) tail = nullptr;
            read_tcb->next_blocked = nullptr;
            return read_tcb;
        }
    } // namespace utils
};