// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VectOS — Lightweight preemptive RTOS for RP2040
// https://github.com/Ferdinaelectro1/VectOS

#include "vecos/scheduler.h"
#include "vecos/platform_timer.h"
#include "vecos/port.h"

extern "C" {
    void start_first_task(TCB **);
};

static void vecos_idle_task_fn(void* arg) {
    (void)arg;
    while (true) {
        vecos::port::put_cpu_to_sleep(); 
    }
}

static vecos::Task<128> idle_task(vecos_idle_task_fn);

TCB* current_task_tcb_ptr = nullptr; //global pour l'asm
vecos::Scheduler* instance_scheduler = nullptr; //global pour l'asm


vecos::Scheduler::Scheduler() : _task_count(0){
    instance_scheduler = this;
    _sys_time = internal_platform_clock;
    // On force la tâche Idle à l'index 0 dès le départ
    _tasks[0] = &idle_task;
    _tasks[0]->get_tcb()->state = TaskState::READY;
    _tasks[0]->get_tcb()->wake_up_time = 0;
    _task_count = 1;
}

bool vecos::Scheduler::add_task(TaskBase &task)
{
    if(_task_count >= HARD_MAX_TASK) return false;
    _tasks[_task_count] = &task;
    task.get_tcb()->wake_up_time = 0;
    task.get_tcb()->state = TaskState::READY;
    _task_count++;
    return true;
}

extern "C" void vTaskSwitchContext() {
    if (instance_scheduler == nullptr || instance_scheduler->task_count() == 0) return;

    uint64_t current_time = 0;
    if (instance_scheduler->_sys_time != nullptr) {
        current_time = instance_scheduler->_sys_time->get_ticks_us(); // Décentralisé !
    }

    uint16_t starting_idx = instance_scheduler->_current_task_idx;
    uint16_t next_idx = starting_idx;

    for (uint16_t i = 1; i < instance_scheduler->task_count(); i++) {
            next_idx = (next_idx + 1) % instance_scheduler->task_count();

            TCB* tcb = instance_scheduler->_tasks[next_idx]->get_tcb();

            // Si la tâche était bloquée, on vérifie si l'heure du réveil a sonné
            if (tcb->state == TaskState::BLOCKED) {
                if (current_time >= tcb->wake_up_time) {
                    tcb->state = TaskState::READY;
                }
            }

            // Si on trouve une tâche prête, on effectue le switch !
            if (tcb->state == TaskState::READY) {
                instance_scheduler->_current_task_idx = next_idx;
                current_task_tcb_ptr = tcb;
                return; 
            }
    }
    //if all task sleeping we go to run idle task
    instance_scheduler->_current_task_idx = 0;
    current_task_tcb_ptr = instance_scheduler->_tasks[0]->get_tcb();
}

void vecos::Scheduler::start()
{
    _current_task_idx = (_task_count > 1) ? 1 : 0;
    current_task_tcb_ptr = _tasks[_current_task_idx]->get_tcb();// get_tcb retourne déjà un * vers un TCB
    vecos::port::init_hardware_context();//init hardware interrupt and ticks timer
    start_first_task(&current_task_tcb_ptr);
}

uint16_t vecos::Scheduler::task_count() const
{
    return _task_count;
}

void vecos::sleep_task_ms(uint32_t ms)
{
    TCB *current = current_task_tcb_ptr;
    if (current && instance_scheduler && instance_scheduler->_sys_time) {
        uint64_t now = instance_scheduler->_sys_time->get_ticks_us();
        current->wake_up_time = now + (static_cast<uint64_t>(ms) * 1000);
        current->state = TaskState::BLOCKED;        
        vecos::port::yield_cpu();
    }
}