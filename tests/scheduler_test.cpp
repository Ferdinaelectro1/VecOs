#include <gtest/gtest.h>
#include "vecos/tcb.hpp"
#include "vecos/scheduler.h"

extern TCB *current_task_tcb_ptr;

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


