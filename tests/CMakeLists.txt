cmake_minimum_required(VERSION 3.13)
project(vecos_tests CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(scheduler_test scheduler_test.cpp)

target_link_libraries(scheduler_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(scheduler_test)