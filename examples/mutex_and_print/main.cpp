#include <stdio.h>
#include <pico/stdlib.h>
#include "vecos/scheduler.h"
#include "vecos/mutex.h"



vecos::Mutex mutex;

void print1(void *arg) {
    const char *str = "[TASK 1] --- Benin is talking ---\r\n";
    while (1) {
        mutex.lock();
        gpio_put(28, 1); // Allume la LED
        
        for (int i = 0; str[i] != '\0'; i++) {
            uart_putc_raw(uart0, str[i]);
            sleep_ms(100);
        }
        
        mutex.unlock();
        vecos::sleep_task_ms(500); // Laisse un vrai temps d'observation de 500ms !
    }    
}

void print2(void *arg) {
    const char *str = "[TASK 2] *** China is talking ***\r\n";
    while (1) {
        mutex.lock();
        gpio_put(28, 0); // Éteint la LED
        
        for (int i = 0; str[i] != '\0'; i++) {
            uart_putc_raw(uart0, str[i]);
            sleep_ms(100);
        }
        
        mutex.unlock();
        vecos::sleep_task_ms(500); // Laisse la Task 1 s'exprimer pendant 500ms
    }    
}

vecos::Task<1024> task1(print1);
vecos::Task<1024> task2(print2);

vecos::Scheduler os;

int main() {
    stdio_init_all();
    gpio_init(28);
    gpio_set_dir(28,GPIO_OUT);
    mutex.init_debug();
    os.add_task(task1);
    os.add_task(task2);
    os.start();
    while (1)
    {
    }    
}