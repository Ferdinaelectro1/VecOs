// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VecOs — Lightweight preemptive RTOS for 32-bit microcontrollers
// https://github.com/Ferdinaelectro1/VecOs

/*
 * Priority inversion demo (the bug that reset the Mars Pathfinder in 1997).
 *
 * Three tasks share one mutex:
 *   - LOW      takes the mutex and does about 1 s of work inside it
 *   - CRITICAL wakes up after 100 ms and asks for the mutex
 *   - NORMAL   wakes up after 200 ms, then burns the CPU for 3 s, in a loop
 *
 * NORMAL has a higher priority than LOW, so it keeps taking the CPU away
 * from LOW. LOW never gets enough time to release the mutex, and CRITICAL
 * waits for it although it has the highest priority.
 *
 * Expected output (read the first line only, the next cycles overlap):
 *   without priority inheritance: about 12900 ms (measured on Wokwi)
 *   with priority inheritance:    about 900 ms   (target)
 */

#include <pico/stdlib.h>
#include <stdio.h>

#include "vecos/scheduler.h"
#include "vecos/mutex.h"

#define LED 27

vecos::Mutex mutex;

//we use the for loop, because, in the pico-sdk, the busy_wait_ms, watch just 
//the timer. Here we count , by intervall of 10 ms, not depend on timer 
//value directly, that allows to avoid counting timer, during another task running
void real_wait_without_just_watch_timer(const uint32_t ms) {
    for(int i = 0; i < ms/10; i++) {
        busy_wait_ms(10);
    }
}

void normal_task_fn() {
    gpio_init(LED);
    gpio_set_dir(LED,GPIO_OUT);
    bool led_state = true;
    gpio_put(LED,led_state);
    while (1)
    {
        vecos::sleep_task(200);
        gpio_put(LED,led_state);
        led_state = !led_state;
        real_wait_without_just_watch_timer(3000);
    }
    
}

void critical_task_fn() {
    while (1)
    {
        vecos::sleep_task(100);
        printf("Start Waiting mutex in critical task\n");
        const auto start = to_ms_since_boot(get_absolute_time());
        mutex.lock();
        const auto end = to_ms_since_boot(get_absolute_time());
        mutex.unlock();
        printf("Mutex free after : %d ms\n",(end - start));
    }
}

void low_task_fn() {
    while (1)
    {
        mutex.lock();
        real_wait_without_just_watch_timer(1000);
        mutex.unlock();
    }
}

vecos::Task<1024> normal_task(normal_task_fn,TaskPriority::NORMAL);
vecos::Task<1024> critical_task(critical_task_fn,TaskPriority::CRITICAL);
vecos::Task<1024> low_task(low_task_fn,TaskPriority::LOW);

vecos::Scheduler os;

int main() {
    stdio_init_all();

    os.add_task(low_task);
    os.add_task(critical_task);
    os.add_task(normal_task);

    os.start();
    while (true)
    {
        /* code */
    }
}