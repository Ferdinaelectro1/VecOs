// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VecOs — Lightweight preemptive RTOS for 32-bit microcontrollers
// https://github.com/Ferdinaelectro1/VecOs

#pragma once

namespace vecos {
    class ListNode {
      public:
        ListNode *next = nullptr;
    }; 

    class LinkedList {
        public:
          LinkedList();
          void push(ListNode* value);
          ListNode* read();
          bool empty() const;

        private:
          ListNode* _head;
          ListNode* _tail;

    };
};