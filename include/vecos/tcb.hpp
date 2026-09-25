// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VectOS — Lightweight preemptive RTOS for Microcontroller
// https://github.com/Ferdinaelectro1/VectOS

#pragma once
#include <stdint.h>

enum class TaskState { READY,SLEEPING,BLOCKED };

enum class TaskPriority : uint8_t {
  LOW      = 0,
  NORMAL   = 1,
  HIGH     = 2,
  CRITICAL = 3
};

//Don't modifiy this struct order because asm file use 
//this order to acces stack_ptr
//If you want new parameter put this at end
struct TCB {
  uint32_t *stack_ptr;
  uint32_t *stack_base;
  uint32_t  stack_size;
  TaskState state = TaskState::READY;
  uint64_t  wake_up_time = 0;
  TCB       *next_blocked;
  TaskPriority priority = TaskPriority::NORMAL;
};