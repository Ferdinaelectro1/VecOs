#include <cstddef>
#include "../include/vecos/tcb.hpp"


int main() {
    static_assert(offsetof(TCB,stack_ptr) == 0,"FAIL: stack_ptr must be at offset 0 !");
}