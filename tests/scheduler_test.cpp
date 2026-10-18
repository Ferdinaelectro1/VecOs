#include <gtest/gtest.h>
#include "vecos/tcb.hpp"
#include "vecos/scheduler.h"

extern TCB *current_task_tcb_ptr;
extern uint64_t g_fake_time_us;

TEST(Scheduler, Tcb_struct) {
    EXPECT_EQ(offsetof(TCB,stack_ptr), 0) << "Error stack_ptr must be at offset 0 !";
}

TEST(Scheduler,HighPriority) {
    vecos::Scheduler os;
    vecos::Task<200> high([](){},TaskPriority::HIGH);
    vecos::Task<200> low([](){},TaskPriority::LOW);

    os.add_task(high);
    os.add_task(low);

    current_task_tcb_ptr = low.get_tcb(); //We init the first task
    
    vTaskSwitchContext();
    EXPECT_EQ(current_task_tcb_ptr,high.get_tcb());
    vTaskSwitchContext();
    EXPECT_EQ(current_task_tcb_ptr,high.get_tcb());

    high.get_tcb()->state = TaskState::BLOCKED;

    vTaskSwitchContext();
    EXPECT_EQ(current_task_tcb_ptr,low.get_tcb());
}


TEST (Scheduler,SleepingTaskWakesUpAtDeadline) {
    current_task_tcb_ptr = nullptr;
    g_fake_time_us = 0;

    vecos::Scheduler os;
    vecos::Task<256> task1([](){});  
    vecos::Task<256> task2([](){});  

    os.add_task(task1);
    os.add_task(task2);
    
    current_task_tcb_ptr = task1.get_tcb();
    vTaskSwitchContext();
    EXPECT_EQ(current_task_tcb_ptr,task1.get_tcb());

    vecos::sleep_task(200);//we put the current task to sleep for 200 ms
    EXPECT_EQ(task1.get_tcb()->state,TaskState::SLEEPING); //check that task1 is really sleeping
    
    vTaskSwitchContext();
    EXPECT_EQ(current_task_tcb_ptr,task2.get_tcb());
    vTaskSwitchContext();
    EXPECT_EQ(current_task_tcb_ptr,task2.get_tcb());
    vTaskSwitchContext();
    EXPECT_EQ(current_task_tcb_ptr,task2.get_tcb());

    g_fake_time_us = 200*1000 - 1; //1 us before the deadline: the task is still asleep
    vTaskSwitchContext();
    EXPECT_EQ(current_task_tcb_ptr,task2.get_tcb());

    g_fake_time_us = 200*1000; //exactly at 200 ms: the task wakes up
    vTaskSwitchContext();
    EXPECT_EQ(task1.get_tcb()->state,TaskState::READY); //the scheduler has really made task1 READY
    EXPECT_EQ(current_task_tcb_ptr,task1.get_tcb());
}
