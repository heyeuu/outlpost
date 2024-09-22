#include "entrypoint.hh"

#include "hal/base/gpio.hh"
#include "hal/base/timer.hh"

void entrypoint() {
    using namespace hal;

    gpio::PH<10> blue {};
    gpio::PH<11> green {};
    gpio::PH<12> red {};

    while (true) {
        time::delay(1000);
        gpio::toggle(green, blue, red);
    }
}
