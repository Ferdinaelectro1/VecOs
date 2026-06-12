add_executable(mutex_and_print main.cpp)

target_link_libraries(mutex_and_print vecos)

pico_add_extra_outputs(mutex_and_print)