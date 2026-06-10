#include "vecos/scheduler.h"
#include "pico/time.h"
#include "hardware/structs/scb.h"

extern "C" {
    void SVC_Handler(void);
    void PendSV_Handler(void);
    void start_first_task(TCB **);
};

TCB* current_task_tcb_ptr = nullptr; //global pour l'asm
vecos::Scheduler* instance_scheduler = nullptr; //global pour l'asm

struct repeating_timer timer;

vecos::Scheduler::Scheduler() : _task_count(0){
    instance_scheduler = this;
}

bool vecos::Scheduler::add_task(TaskBase &task)
{
    if(_task_count >= HARD_MAX_TASK) return false;
    _tasks[_task_count] = &task;
    _task_count++;
    return true;
}

extern "C" void vTaskSwitchContext() {
    if (instance_scheduler == nullptr || instance_scheduler->task_count() == 0) return;

    uint16_t next_idx = instance_scheduler->_current_task_idx + 1;

    if(next_idx >= instance_scheduler->task_count()) 
    {
        next_idx = 0;
    }

    instance_scheduler->_current_task_idx = next_idx;

    current_task_tcb_ptr = instance_scheduler->_tasks[next_idx]->get_tcb();
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
    current_task_tcb_ptr = _tasks[0]->get_tcb();// get_tcb retourne déjà un * vers un TCB
    initSchedulerTimer_andPendCV();
    start_first_task(&current_task_tcb_ptr);
}

uint16_t vecos::Scheduler::task_count() const
{
    return _task_count;
}
