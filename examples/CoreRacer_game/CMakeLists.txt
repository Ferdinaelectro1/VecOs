add_executable(CoreRacer_game main.cpp)

target_link_libraries(CoreRacer_game hardware_i2c vecos)

pico_add_extra_outputs(CoreRacer_game)