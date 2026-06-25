#pragma once
#include <cstddef>
#include "vecos/port.h"
#include "vecos/scheduler.h"

extern TCB* current_task_tcb_ptr;

namespace vecos {

    template<typename T, size_t QUEUE_SIZE> 
    class Queue {
        private:
           T _buffer[QUEUE_SIZE];
           size_t _head = 0; // Write index
           size_t _tail = 0; // Read index
           size_t _count = 0; // Current number of elements in the queue

           TCB* _waiting_to_read  = nullptr; // List of tasks blocked waiting to read (queue empty)
           TCB* _waiting_to_write = nullptr; // List of tasks blocked waiting to write (queue full)

        public:
           Queue() = default;

           void send(const T& item);
           void receive(T& item);
    };

    template <typename T, size_t QUEUE_SIZE>
    inline void Queue<T, QUEUE_SIZE>::send(const T &item)
    {
        uint32_t intState = vecos::port::save_and_disable_interrupts();
        
        // If the queue is full, block the producing task
        while (_count >= QUEUE_SIZE) {
            current_task_tcb_ptr->state = TaskState::BLOCKED;
            current_task_tcb_ptr->next_blocked = nullptr;
            
            if(!_waiting_to_write) {
                _waiting_to_write = current_task_tcb_ptr; 
            } else {
                TCB *iterator = _waiting_to_write;
                while (iterator->next_blocked != nullptr) {
                    iterator = iterator->next_blocked;
                }
                iterator->next_blocked = current_task_tcb_ptr;
            }
            
            vecos::port::restore_interrupts(intState);
            vecos::port::yield_cpu();
            
            // Upon waking up, resume here. Re-disable interrupts to re-evaluate the while loop condition
            intState = vecos::port::save_and_disable_interrupts();
        } 

        // Write data into the circular buffer
        _buffer[_head] = item;
        _head = (_head + 1) % QUEUE_SIZE;
        _count++;

        // WAKE UP: If a consumer task is waiting, wake up the first one in the list
        if (_waiting_to_read != nullptr) {
            TCB *task_to_wake = _waiting_to_read;
            _waiting_to_read = _waiting_to_read->next_blocked; // Remove it from the waiting list
            task_to_wake->next_blocked = nullptr;
            task_to_wake->state = TaskState::READY;           // Set task state to ready
        }

        vecos::port::restore_interrupts(intState);
    }

    template <typename T, size_t QUEUE_SIZE>
    inline void Queue<T, QUEUE_SIZE>::receive(T &item)
    {
        uint32_t intState = vecos::port::save_and_disable_interrupts();
        
        // If the queue is empty, block the consuming task
        while (_count == 0) {
            current_task_tcb_ptr->state = TaskState::BLOCKED;
            current_task_tcb_ptr->next_blocked = nullptr;
            
            if(!_waiting_to_read) {
                _waiting_to_read = current_task_tcb_ptr; 
            } else {
                TCB *iterator = _waiting_to_read;
                while (iterator->next_blocked != nullptr) {
                    iterator = iterator->next_blocked;
                }
                iterator->next_blocked = current_task_tcb_ptr;
            }
            
            vecos::port::restore_interrupts(intState);
            vecos::port::yield_cpu();          
            
            // Upon waking up, resume here. Re-disable interrupts to re-evaluate the while loop condition
            intState = vecos::port::save_and_disable_interrupts();
        }

        // Read data from the circular buffer
        item = _buffer[_tail];
        _tail = (_tail + 1) % QUEUE_SIZE;
        _count--;

        // WAKE UP: If a producer task was blocked because the queue was full, wake it up
        if (_waiting_to_write != nullptr) {
            TCB *task_to_wake = _waiting_to_write;
            _waiting_to_write = _waiting_to_write->next_blocked; // Remove it from the waiting list
            task_to_wake->next_blocked = nullptr;
            task_to_wake->state = TaskState::READY;             // Set task state to ready
        }

        vecos::port::restore_interrupts(intState);
    }

} // End vecos