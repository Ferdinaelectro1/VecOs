// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VecOs — Lightweight preemptive RTOS for 32-bit microcontrollers
// https://github.com/Ferdinaelectro1/VecOs

#include <gtest/gtest.h>
#include <functional>
#include <csetjmp>
#include "vecos/scheduler.h"
#include "vecos/mutex.h"

extern TCB *current_task_tcb_ptr;
extern std::jmp_buf environment;
extern bool jump_armed;

static void lock_until_blocked (vecos::Mutex &mutex) {
    jump_armed = false;
    if(setjmp(environment) == 0) {
        jump_armed = true;
        mutex.lock();
    }
    jump_armed = false;
}

static void unlock_until (vecos::Mutex &mutex) {
    mutex.unlock();
}

TEST(Mutex,InheritanceOfPriority) {
    vecos::Mutex mutex;
    current_task_tcb_ptr = nullptr;

    std::function<void(vecos::Mutex &)> task_critical_func_ptr;
    std::function<void(vecos::Mutex &)> task_low_func_ptr;

    task_critical_func_ptr = lock_until_blocked;
    task_low_func_ptr = lock_until_blocked;
    vecos::Task<256> task_critical([](){},TaskPriority::CRITICAL);  
    vecos::Task<256> task_low([](){},TaskPriority::LOW);   

    current_task_tcb_ptr = task_low.get_tcb();
    task_low_func_ptr(mutex);
    current_task_tcb_ptr = task_critical.get_tcb();
    task_critical_func_ptr(mutex);

    /*
      if low high task try to lock mutex, and the mutex is already take by low task
      We increase the priority of low task
    */
    EXPECT_EQ(task_low.get_tcb()->priority, TaskPriority::CRITICAL);
    EXPECT_EQ(task_low.get_tcb()->base_priority, TaskPriority::LOW);
    EXPECT_EQ(task_critical.get_tcb()->priority, TaskPriority::CRITICAL);
    EXPECT_EQ(task_critical.get_tcb()->base_priority, TaskPriority::CRITICAL);

    task_critical_func_ptr = unlock_until;
    task_low_func_ptr = unlock_until;

    current_task_tcb_ptr = task_low.get_tcb();
    task_low_func_ptr(mutex);
    current_task_tcb_ptr = task_critical.get_tcb();
    task_critical_func_ptr(mutex);

    /*
      after unlocking of mutex by low task that priority , was increase,
      we ensure that , the priority of low task reverts to low
    */
    EXPECT_EQ(task_low.get_tcb()->priority, TaskPriority::LOW);
    EXPECT_EQ(task_low.get_tcb()->base_priority, TaskPriority::LOW);
    EXPECT_EQ(task_critical.get_tcb()->priority, TaskPriority::CRITICAL);
    EXPECT_EQ(task_critical.get_tcb()->base_priority, TaskPriority::CRITICAL);
}