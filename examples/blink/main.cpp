#include <stdio.h>
#include <pico/stdlib.h>
#include "hardware/structs/scb.h"
#include <vecos/scheduler.h>

#define LED1 28
#define LED2 27
#define LED3 26

void Task1 (void* arg) {
   (void)arg; // Évite le warning d'inutilisation 
    gpio_init(LED1);
    gpio_set_dir(LED1, GPIO_OUT);

    while (true) {
        gpio_put(LED1, 1);
        vecos::sleep_task_ms(1000);
        gpio_put(LED1, 0);
        vecos::sleep_task_ms(1000); 
    }
}

void Task2(void* arg) {
   (void)arg; // Évite le warning d'inutilisation 
    gpio_init(LED2);
    gpio_set_dir(LED2, GPIO_OUT);
    while (true) {
        gpio_put(LED2, 0);
        vecos::sleep_task_ms(500);
        gpio_put(LED2, 1);
        vecos::sleep_task_ms(500);
    }
}

void Task3(void* arg) {
   (void)arg; // Évite le warning d'inutilisation 
    gpio_init(LED3);
    gpio_set_dir(LED3, GPIO_OUT);
    while (true) {
        gpio_put(LED3, 0);
        vecos::sleep_task_ms(250);
        gpio_put(LED3, 1);
        vecos::sleep_task_ms(250);
    }
}

vecos::Task<1024> task1(Task1);
vecos::Task<1024> task2(Task2);
vecos::Task<1024> task3(Task3);


vecos::Scheduler os;

int main() {
    os.add_task(task1);
    os.add_task(task2);
    os.add_task(task3);
    os.start();
    while(1) {
        printf("Erreur start scheduler !\n");
        sleep_ms(1000);
    } 
}