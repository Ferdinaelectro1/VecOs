// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VecOs — Lightweight preemptive RTOS for 32-bit microcontrollers
// https://github.com/Ferdinaelectro1/VecOs

#include "vecos/mutex.h"
#include "vecos/port.h"

extern TCB* current_task_tcb_ptr;

vecos::Mutex::Mutex() {
    _locked = false;
    _owner  = nullptr;
}

void vecos::Mutex::lock()
{
    //we disable hardware interrupt doing mutex operation (that is atomics operation)
    uint32_t inter_state = vecos::port::save_and_disable_interrupts();

    if(_locked) {
        /*
          If the priority of current waiting task is more than _owner task, we increase, 
          the priority, of _owner task, to the same priority.
        */
        if(current_task_tcb_ptr->priority > _owner->priority) {
            _owner->priority = current_task_tcb_ptr->priority;
        }
        vecos::utils::append(_head,_tail,current_task_tcb_ptr);

        while(_locked){
            current_task_tcb_ptr->state = TaskState::BLOCKED;
            vecos::port::yield_cpu(); //we yield cpu to continue running another task
            vecos::port::restore_interrupts(inter_state); //restore interrupt
            inter_state = vecos::port::save_and_disable_interrupts(); //we relock interrupt befor verifie mutex
        } 
    }
 

    _locked = true;
    _owner = current_task_tcb_ptr;
    
    vecos::port::restore_interrupts(inter_state); //after lock we restore hardware interrupt
}

void vecos::Mutex::unlock() {
    uint32_t inter_state = vecos::port::save_and_disable_interrupts();
    if (_owner == current_task_tcb_ptr) {
        _owner->priority = _owner->base_priority;
    }
    _owner = nullptr;
    _locked = false;
    //we wake up a single task
    TCB *task_to_wake = vecos::utils::read_front(_head,_tail);
    if(task_to_wake != nullptr) {
        task_to_wake->state = TaskState::READY;
        vecos::port::yield_cpu(); // yield so the scheduler can run the woken task right away if it has a higher priority
    }
    vecos::port::restore_interrupts(inter_state); //after lock we restore hardware interrupt
}

bool vecos::Mutex::try_lock() {
    uint32_t inter_state = vecos::port::save_and_disable_interrupts();
    if(_locked) 
    {
        vecos::port::restore_interrupts(inter_state);
        return false;
    }
    _locked = true;
    _owner = current_task_tcb_ptr;
    vecos::port::restore_interrupts(inter_state);
    return true;
}