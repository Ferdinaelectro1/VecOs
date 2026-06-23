#include <stdio.h>
#include <pico/stdlib.h>
#include "vecos/scheduler.h"
#include "vecos/semaphore.h"

#define LED 28

vecos::Semaphore semaphore(0);
int bus = 0;

void Producer_A() {
    while (1) {
        bus++;
        printf("[Prod A] Created data: %d\n", bus);
        semaphore.signal();
        vecos::sleep_task_ms(400); 
    }    
}

void Producer_B() {
    while (1) {
        bus++;
        printf("[Prod B] Created data: %d\n", bus);
        semaphore.signal();
        vecos::sleep_task_ms(600); 
    }    
}

void Consumer_1() {
    while (1) {
        semaphore.wait();
        printf("  -> [Cons 1] Handled value = %d\n", bus);
        vecos::sleep_task_ms(20); 
    }   
}

void Consumer_2() {
    while (1) {
        semaphore.wait();
        printf("  -> [Cons 2] Handled value = %d\n", bus);
        vecos::sleep_task_ms(20);
    }   
}

void Consumer_3() {
    while (1) {
        semaphore.wait();
        printf("  -> [Cons 3] Handled value = %d\n", bus);
        vecos::sleep_task_ms(20);
    }   
}

void blink_task() {
    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);
    while(1) {
        gpio_put(LED, 1);
        vecos::sleep_task_ms(100);
        gpio_put(LED, 0);
        vecos::sleep_task_ms(100);
    }
}


vecos::Task<512> task_cons1(Consumer_1);
vecos::Task<512> task_cons2(Consumer_2);
vecos::Task<512> task_cons3(Consumer_3);
vecos::Task<512> task_prodA(Producer_A);
vecos::Task<512> task_prodB(Producer_B);
vecos::Task<512> task_blink(blink_task);

vecos::Scheduler os;

int main() {
    stdio_init_all();
    printf("--- Starting VectOS Multi-Task Test ---\n");

    os.add_task(task_cons1);
    os.add_task(task_cons2);
    os.add_task(task_cons3);
    os.add_task(task_prodA);
    os.add_task(task_prodB);
    os.add_task(task_blink);

    os.start();

    while(1) {
        printf("Scheduler failed to start successfully!\n");
        sleep_ms(1000);
    };
}