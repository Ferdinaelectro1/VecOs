// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VectOS — Lightweight preemptive RTOS for RP2040
// https://github.com/Ferdinaelectro1/VectOS

#pragma once
#include "utils.h"
#include "platform_timer.h"
#include "port.h"

#include <stdint.h>

extern "C" void vTaskSwitchContext();

enum class TaskState { READY,SLEEPING,BLOCKED };

//Don't modifiy this struct order because asm file use 
//this order to acces stack_ptr
//If you want new parameter put this at end
struct TCB {
  uint32_t *stack_ptr;
  uint32_t *stack_base;
  uint32_t  stack_size;
  TaskState state = TaskState::READY;
  uint64_t  wake_up_time = 0;
};


namespace vecos {

    class TaskBase {
        public:
          TaskBase(const TaskBase &) = delete;
          TaskBase operator=(const TaskBase &) = delete;
          TCB* get_tcb() {
            return &_tcb;
          }

        protected: 
          TaskBase(uint32_t* stack_top,uint32_t size) {
            _tcb.stack_base = stack_top;
            _tcb.stack_size = size;
          }

        private:
          TCB _tcb;
    };

    template<uint16_t stack_size = 1024>
    class Task : public TaskBase {
        public:
          Task(void (*task_fn)(void *),void *arg = nullptr) : 
          TaskBase(&_stack[stack_size-1],stack_size)
          {
            vecos::port::task_init(task_fn,arg,this->get_tcb());
          } 


          Task(void (*task_fn)(),void *arg = nullptr) : 
          TaskBase(&_stack[stack_size-1],stack_size)
          {
            auto casted_fn = reinterpret_cast<void (*)(void *)>(task_fn);
            vecos::port::task_init(casted_fn,arg,this->get_tcb());
          } 

        private:
          uint32_t _stack[stack_size];
    };

    void sleep_task_ms(uint32_t ms);

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
          friend void sleep_task_ms(uint32_t ms);
    };
    
}