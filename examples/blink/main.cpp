#include <stdio.h>
#include <pico/stdlib.h>
#include "hardware/structs/scb.h"
#include <vecos/scheduler.h>

#define LED1 28
#define LED2 27

static void delay_software(volatile uint32_t count) {
    while(count--) {
        __asm volatile("nop");
    }
}

void Task1 (void* arg) {
   (void)arg; // Évite le warning d'inutilisation 
    gpio_init(LED1);
    gpio_set_dir(LED1, GPIO_OUT);

    while (true) {
        gpio_put(LED1, 1);
        delay_software(2500000);
        gpio_put(LED1, 0);
        delay_software(2500000);  
    }
}

void Task2(void* arg) {
   (void)arg; // Évite le warning d'inutilisation 
    gpio_init(LED2);
    gpio_set_dir(LED2, GPIO_OUT);
    while (true) {
        gpio_put(LED2, 0);
        delay_software(5000000);
        gpio_put(LED2, 1);
        delay_software(5000000);
    }
}

vecos::Task<1024> task1(Task1);
vecos::Task<1024> task2(Task2);

vecos::Scheduler os;

int main() {
    os.add_task(task1);
    os.add_task(task2);
    os.start();
    while(1) {
        printf("Erreur start scheduler !\n");
        sleep_ms(1000);
    } 
}