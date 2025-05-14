#pragma once

#include <zephyr/drivers/gpio.h>

namespace ztemplate::led_toggler
{
    class LedToggler
    {
        public:
        explicit LedToggler(gpio_dt_spec led);

        bool init();
        void toggle();

        private:
        gpio_dt_spec _led = {};
    };
}    // namespace ztemplate::led_toggler
