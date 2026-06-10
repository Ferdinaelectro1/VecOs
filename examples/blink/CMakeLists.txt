add_executable(blink main.cpp)

target_link_libraries(blink vecos)

pico_add_extra_outputs(blink)