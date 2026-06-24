// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VectOS — Lightweight preemptive RTOS for Microcontroller
// https://github.com/Ferdinaelectro1/VectOS

#include "vecos/semaphore.h"
#include "vecos/port.h"

extern TCB* current_task_tcb_ptr;

vecos::Semaphore::Semaphore(uint32_t initial_count): _counter(initial_count)
{
    _waiting_tcb = nullptr;
}

void vecos::Semaphore::wait()
{
    //Atomic operation
    uint32_t inter_flag = vecos::port::save_and_disable_interrupts();
    
    /**
     * We have a free semaphore
     */
    if(_counter > 0) {
        _counter--;
        vecos::port::restore_interrupts(inter_flag);
        return;
    }

    current_task_tcb_ptr->state = TaskState::BLOCKED;
    current_task_tcb_ptr->next_blocked = nullptr;
    
    if(!_waiting_tcb) _waiting_tcb = current_task_tcb_ptr; //first waiting tcb
    else {
        TCB *iterator = _waiting_tcb;
        while(iterator->next_blocked != nullptr) {
            iterator = iterator->next_blocked;
        }
        iterator->next_blocked = current_task_tcb_ptr;
    }

    vecos::port::restore_interrupts(inter_flag);
    vecos::port::yield_cpu();
}

void vecos::Semaphore::signal()
{
    //Atomic operation
    uint32_t inter_flag = vecos::port::save_and_disable_interrupts();

    /**
     * No waiting task
     */
    if(!_waiting_tcb) {
        _counter++;
        vecos::port::restore_interrupts(inter_flag);
        return;
    }

    TCB *task_to_wake = _waiting_tcb;
    _waiting_tcb = _waiting_tcb->next_blocked;
    task_to_wake->state  = TaskState::READY;
    task_to_wake->next_blocked = nullptr;
    vecos::port::restore_interrupts(inter_flag);
}
