// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VectOS — Lightweight preemptive RTOS for RP2040
// https://github.com/Ferdinaelectro1/VectOS

#pragma once
#include "utils.h"
#include "platform_timer.h"

#include <stdint.h>

extern "C" void vTaskSwitchContext();

enum class TaskState { READY, BLOCKED };

typedef struct {
  uint32_t *stack_ptr;
  uint32_t *stack_base;
  uint32_t  stack_size;
  uint64_t  wake_up_time = 0;
  TaskState state = TaskState::READY;
} TCB;

static void task_init(void (*task_func_ptr)(void *args),TCB *task_tcb) {
  uint32_t *sp = task_tcb->stack_base; //on initialise le pointeur de pile au sommet de la pile
  *sp-- = 0x01000000; //on charge xpsr l'état du reg de statut xpsr puis on décrémente le pointeur de pile
  *sp-- = (uint32_t )task_func_ptr;//on charge pc en y mettant l'adresse de la fonction de cette tâche
  *sp-- = 0;                        // LR
  *sp-- = 0;                        // R12
  *sp-- = 0;                        // R3
  *sp-- = 0;                        // R2
  *sp-- = 0;                        // R1
  *sp-- = 0;                        // R0

  *sp-- = 0;                        // R11
  *sp-- = 0;                        // R10
  *sp-- = 0;                        // R9
  *sp-- = 0;                        // R8

  *sp-- = 0;                        // R7
  *sp-- = 0;                        // R6
  *sp-- = 0;                        // R5
  *sp   = 0;                        // R4
  task_tcb->stack_ptr = sp;
}

namespace vecos {

    class TaskBase {
        public:
          TaskBase(const TaskBase &) = delete;
          TaskBase operator=(const TaskBase &) = delete;
          TCB* get_tcb() {
            return &_tcb;
          }

        protected: 
          TaskBase(uint32_t* stack_top,uint32_t size,void (*task_fn)(void *arg), void *arg = nullptr) {
            _tcb.stack_base = stack_top;
            _tcb.stack_size = size;
             task_init(task_fn,&_tcb);
          }

        private:
          TCB _tcb;
    };

    template<uint16_t stack_size = 1024>
    class Task : public TaskBase {
        public:
          Task(void (*task_fn)(void *), void *arg = nullptr) : 
               TaskBase(&_stack[stack_size - 1],stack_size,task_fn,arg) 
          {
            
          } 

        private:
          uint32_t _stack[stack_size];
    };

    void sleep_ms(uint32_t ms);

    class Scheduler {
        
        public: 
        
          explicit Scheduler();
        
    
          bool add_task(TaskBase& task);
        
    
          [[noreturn]] void start();

    
          uint16_t task_count() const ; 

        private:
          uint16_t _task_count;
          uint16_t _current_task_idx = 0;
          TaskBase *_tasks[HARD_MAX_TASK];
          SystemTime *_sys_time = nullptr;

          friend void ::vTaskSwitchContext();
          friend void sleep_ms(uint32_t ms);
    };
    
}