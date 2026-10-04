#include <stdio.h>
#include <pico/stdlib.h>
#include <vecos/scheduler.h>

#define LED_PIN 27

#define OVERFLOW_DELAY_S 20

extern TCB* current_task_tcb_ptr;

void Func() {
    while (1)
    {
        gpio_put(LED_PIN, 1);
        vecos::sleep_task(250);
        gpio_put(LED_PIN, 0);
        vecos::sleep_task(250);
    }
}

void OverflowFunc() {
    for (int i = 0; i < OVERFLOW_DELAY_S; i++)
    {
        vecos::sleep_task(1000);
    }
    *(current_task_tcb_ptr->stack_base - (current_task_tcb_ptr->stack_size - 1)) = 0x00FF00FF; 
    while (1)
    {
    }
}

vecos::Task<1024> task1(Func);
vecos::Task<1024> task2(OverflowFunc);

vecos::Scheduler os;

int main() {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    printf("%08lx\n", (unsigned long)*(task1.get_tcb()->stack_base - (task1.get_tcb()->stack_size - 1)));
    printf("%08lx\n", (unsigned long)*(task2.get_tcb()->stack_base - (task2.get_tcb()->stack_size - 1)));

    os.add_task(task1);
    os.add_task(task2);
    os.set_stackOverflowHandler([](TCB *tcb){
        printf("TCB stack overflow  = %08lx\n",(unsigned long)*(tcb->stack_base - (tcb->stack_size - 1)));
        while(1);
    });
    os.start();

    while (1)
    {
        printf("Scheduler failed to start successfully!\n");
        sleep_ms(1000);
    }
}