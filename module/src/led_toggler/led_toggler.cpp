#include "ztemplate/led_toggler/led_toggler.h"

#include <utility>

namespace ztemplate::led_toggler
{
    LedToggler::LedToggler(gpio_dt_spec led)
        : _led(led)
    {}

    bool LedToggler::init()
    {
        if (!gpio_is_ready_dt(&_led))
        {
            return false;
        }

        if (gpio_pin_configure_dt(&_led, GPIO_OUTPUT_ACTIVE) < 0)
        {
            return false;
        }

        return true;
    }

    void LedToggler::toggle()
    {
        gpio_pin_toggle_dt(&_led);
    }
}    // namespace ztemplate::led_toggler
