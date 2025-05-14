#include "ztemplate/led_toggler/led_toggler.h"

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

namespace
{
    LOG_MODULE_REGISTER(main);

    constexpr uint32_t SLEEP_TIME_MS = 100;
}    // namespace

int main()
{
    auto ledTogglerInstance = ztemplate::led_toggler::LedToggler(GPIO_DT_SPEC_GET(DT_ALIAS(toggle_led), gpios));

    if (!ledTogglerInstance.init())
    {
        LOG_ERR("LED init failed");
        return 0;
    }

    while (1)
    {
        ledTogglerInstance.toggle();
        LOG_INF("LED toggled");
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}
