#include <gtest/gtest.h>
#include "../include/vecos/tcb.hpp"

TEST(Scheduler, Tcb_struct) {
    EXPECT_EQ(offsetof(TCB,stack_ptr), 0) << "Error stack_ptr must be at offset 0 !";
}

