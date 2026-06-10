// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Ferdinand ATI
// VectOS — Lightweight preemptive RTOS for RP2040
// https://github.com/Ferdinaelectro1/VectOS

#pragma once
#include <stdint.h>

namespace vecos {

    class SystemTime {
    public:
        virtual uint64_t get_ticks_us() = 0;
        virtual ~SystemTime() = default;
    };

#if defined(PICO_BOARD) || defined(TARGET_RP2040)
    #include "pico/time.h"
    
    class RP2040Clock : public SystemTime {
    public:
        uint64_t get_ticks_us() override {
            return time_us_64();
        }
    };

    inline RP2040Clock platform_clock_instance;

#elif defined(STM32F4xx)
    #include "stm32f4xx_hal.h"
    
    class STM32Clock : public SystemTime {
    public:
        uint64_t get_ticks_us() override {
            return HAL_GetTick() * 1000;
        }
    };

    inline STM32Clock platform_clock_instance;

#else
    #error "VectOS error: Unsupported hardware platform or missing platform macro!"
#endif

    inline SystemTime* internal_platform_clock = &platform_clock_instance;

} // namespace vecos