cmake_minimum_required(VERSION 3.13)
project(vecos_tests CXX C)
enable_testing()

add_executable(tcb_layout_test tcb_layout_test.cpp)
add_test(NAME tcb_layout_test COMMAND tcb_layout_test)