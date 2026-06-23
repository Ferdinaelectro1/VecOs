#pragma once
#include <stdint.h>
#include <vecos/scheduler.h>

namespace vecos {
    class Semaphore {
        public:
           Semaphore(uint32_t initial_count = 1);
           void wait();
           void signal();

        private:
           uint32_t _counter;
           TCB *_waiting_tcb;

    };
} //End vecos
