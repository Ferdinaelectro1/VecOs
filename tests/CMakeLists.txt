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

add_executable(scheduler_test scheduler_test.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/vecos/scheduler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fake/fake_port.cpp
)

target_include_directories(scheduler_test PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(scheduler_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(scheduler_test)