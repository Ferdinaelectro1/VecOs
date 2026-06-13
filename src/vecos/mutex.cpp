#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include "vecos/mutex.h"
#include "vecos/port.h"

extern TCB* current_task_tcb_ptr;

vecos::Mutex::Mutex() {
    _locked = false;
    _owner  = nullptr;
}

void vecos::Mutex::init_debug()
{
    gpio_init(LED);
    gpio_set_dir(LED,GPIO_OUT);
}

void vecos::Mutex::lock()
{
    //we disable hardware interrupt doing mutex operation (that is atomics operation)
    uint32_t inter_state = vecos::port::save_and_disable_interrupts();
    bool already_waiting = false;

    //if mutex is already lock 
    while(_locked){
        if(!already_waiting) {
            if(_waiting_mutex_tcb_count < 16) { // Sécurité anti-débordement
                _waiting_mutex_tcb[_waiting_mutex_tcb_count++] = current_task_tcb_ptr;
                already_waiting = true;
            }
        }
        current_task_tcb_ptr->state = TaskState::BLOCKED;
        vecos::port::restore_interrupts(inter_state); //restore interrupt
        vecos::port::yield_cpu(); //we yield cpu to continue running another task
        inter_state = vecos::port::save_and_disable_interrupts(); //we relock interrupt befor verifie mutex
    } 
 
    gpio_put(LED,0);
    _locked = true;
    _owner = current_task_tcb_ptr;
    
    vecos::port::restore_interrupts(inter_state); //after lock we restore hardware interrupt
}

void vecos::Mutex::unlock() {
    gpio_put(LED,1);
    uint32_t inter_state = vecos::port::save_and_disable_interrupts();
    _locked = false;
    _owner = nullptr;
    //we wake up a single task
    if(_waiting_mutex_tcb_count) {
        _waiting_mutex_tcb[--_waiting_mutex_tcb_count]->state = TaskState::READY;
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