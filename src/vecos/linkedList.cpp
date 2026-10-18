// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VecOs — Lightweight preemptive RTOS for 32-bit microcontrollers
// https://github.com/Ferdinaelectro1/VecOs

#include "linkedList.h"

vecos::LinkedList::LinkedList() {
    _head = nullptr;
    _tail = nullptr;
}

void vecos::LinkedList::push(ListNode* value) {
    if(!value) return;
    value->next = nullptr;
    if(_head == nullptr) {
        _head = value;
        _tail = value; 
    } else {
        _tail->next = value;
        _tail = _tail->next;
    }
}

vecos::ListNode* vecos::LinkedList::read() {
    if(_head == nullptr) return nullptr;
    ListNode *read_value = _head;
    _head = _head->next;
    if(_head == nullptr) {
        _tail = nullptr;
    }
    read_value->next = nullptr;
    return read_value;
}

bool vecos::LinkedList::empty() const
{
    return (_head == nullptr);
}
