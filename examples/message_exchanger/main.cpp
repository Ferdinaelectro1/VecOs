#include "stdio.h"
#include "pico/stdlib.h"
#include "vecos/scheduler.h"
#include "vecos/queue_message.h"

vecos::Queue<int,5> queue;
int cnt = 0;

void Sender() {
    while(1) {
        queue.send(cnt);
        printf("[Sender] Sent %d, space left: %size...\n", cnt);
        cnt++;
        vecos::sleep_task_ms(100);
    }
}

void Receiver() {
    while(1) {
        int read;
        queue.receive(read);
        printf("         [Receiver] Read = %d\n", read);
        vecos::sleep_task_ms(300);
    }
}

vecos::Task<512> sender(Sender);
vecos::Task<512> receiver(Receiver);

vecos::Scheduler os;

int main() {
    stdio_init_all();
    os.add_task(sender);
    os.add_task(receiver);
    os.start();
    while (1)
    {
        printf("Error sheduler not started !\n");
        sleep_ms(1000);
    }    
}