#include "system.hh"

#include "hal/base/gpio.hh"
#include "hal/base/timer.hh"

#include "spi.h"

static auto pin_reset = hal::gpio::PB<14> {};
static auto pin_dc = hal::gpio::PB<12> {};

void sys::delay(uint32_t ms) {
    hal::time::delay(ms);
}

void sys::reset() {
    pin_reset.reset();
    delay(10);
    pin_reset.set();
    delay(10);
}

void sys::command(uint8_t byte) {
    pin_dc.reset();
    HAL_SPI_Transmit(&hspi2, (uint8_t*)&byte, 1, UINT32_MAX);
}

void sys::data(uint8_t* buffer, size_t size) {
    pin_dc.set();
    HAL_SPI_Transmit(&hspi2, buffer, size, UINT32_MAX);
}