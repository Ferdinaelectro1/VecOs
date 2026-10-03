#include <stdio.h>
#include <pico/stdlib.h>
#include "vecos/scheduler.h"
#include "vecos/semaphore.h"
#include "vecos/tcb.hpp"

#define LED 26

vecos::Semaphore semaphore(0);
volatile uint32_t t_signal = 0;

void critical_task_func() {
    gpio_init(LED);
    gpio_set_dir(LED,GPIO_OUT);
    gpio_put(LED,0);
    while(1) {
        semaphore.wait();
        uint32_t latency = time_us_32() - t_signal;
        gpio_put(LED,0);
        printf("réveil après %lu us\n", (unsigned long)latency);
    }
}

void low_task_func() {
    gpio_init(LED);
    gpio_set_dir(LED,GPIO_OUT);
    while (1)
    {
        t_signal = time_us_32();
        semaphore.signal();
        gpio_put(LED,1);
        busy_wait_ms(20);
        vecos::sleep_task(2000);
    }
}

vecos::Scheduler os;
vecos::Task<256> task1(critical_task_func,TaskPriority::CRITICAL);
vecos::Task<256> task2(low_task_func,TaskPriority::LOW);

int main() {
    stdio_init_all();
    os.add_task(task1);
    os.add_task(task2);

    os.start();
    while(1) {
        printf("Scheduler failed to start successfully!\n");
        sleep_ms(1000);
    };
}