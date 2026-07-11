// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VectOS — Lightweight preemptive RTOS for Microcontroller
// https://github.com/Ferdinaelectro1/VectOS

#include <pico/stdlib.h>
#include <hardware/gpio.h>
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
        current_task_tcb_ptr->next_blocked = nullptr;
        //if mutex is already lock 
        
        if(_head == nullptr) {
            _head = current_task_tcb_ptr;
            _tail = current_task_tcb_ptr;
        }  else {
            _tail->next_blocked = current_task_tcb_ptr;
            _tail = current_task_tcb_ptr;
        }

        while(_locked){
            current_task_tcb_ptr->state = TaskState::BLOCKED;
            vecos::port::restore_interrupts(inter_state); //restore interrupt
            vecos::port::yield_cpu(); //we yield cpu to continue running another task
            inter_state = vecos::port::save_and_disable_interrupts(); //we relock interrupt befor verifie mutex
        } 
    }
 

    _locked = true;
    _owner = current_task_tcb_ptr;
    
    vecos::port::restore_interrupts(inter_state); //after lock we restore hardware interrupt
}

void vecos::Mutex::unlock() {
    uint32_t inter_state = vecos::port::save_and_disable_interrupts();
    _locked = false;
    //we wake up a single task
    if(_head != nullptr) {
        TCB *task_to_wake = _head;
        _head = _head->next_blocked;
        if(_head == nullptr) _tail = nullptr;
        task_to_wake->state = TaskState::READY;
        task_to_wake->next_blocked = nullptr;
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