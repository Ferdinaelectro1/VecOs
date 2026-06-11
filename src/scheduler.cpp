// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VectOS — Lightweight preemptive RTOS for RP2040
// https://github.com/Ferdinaelectro1/VectOS

#include "pico/time.h"
#include "hardware/structs/scb.h"

#include "vecos/scheduler.h"
#include "vecos/platform_timer.h"

extern "C" {
    void SVC_Handler(void);
    void PendSV_Handler(void);
    void start_first_task(TCB **);
};

static void vecos_idle_task_fn(void* arg) {
    (void)arg;
    while (true) {
        __asm volatile("wfi"); 
    }
}

static vecos::Task<128> idle_task(vecos_idle_task_fn);

TCB* current_task_tcb_ptr = nullptr; //global pour l'asm
vecos::Scheduler* instance_scheduler = nullptr; //global pour l'asm

struct repeating_timer timer;

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

static bool repeating_timer_callback(struct repeating_timer *) {
    __asm volatile("svc #0");//déclenche une interruption qui va appeler le pendCV
    return true;
}

static void initSchedulerTimer_andPendCV () {
    add_repeating_timer_ms(-5,repeating_timer_callback,NULL,&timer);

    // --- INJECTION BRUTE DANS LA TABLE VTOR ---
    // On récupère l'adresse de la table des vecteurs actuellement utilisée en RAM
    uint32_t *vtor_table = (uint32_t *)scb_hw->vtor;
    
    // Case 11 = SVC (Supervisor Call)
    vtor_table[11] = (uint32_t)SVC_Handler;
    
    // Case 14 = PendSV (Pendable Service Call)
    vtor_table[14] = (uint32_t)PendSV_Handler;
    
    // Adresse du registre SHPR3 (System Handler Priority Register 3)
    volatile uint32_t *shpr3 = (volatile uint32_t *)0xE000ED20;

    // On force la priorité de PendSV (bits 16-23) à 0xC0 (priorité la plus basse sur M0+)
    *shpr3 = (*shpr3 & 0xFF00FFFF) | 0x00C00000;
}

void vecos::Scheduler::start()
{
    _current_task_idx = (_task_count > 1) ? 1 : 0;
    current_task_tcb_ptr = _tasks[_current_task_idx]->get_tcb();// get_tcb retourne déjà un * vers un TCB
    initSchedulerTimer_andPendCV();
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
        __asm volatile("svc #0");
    }
}