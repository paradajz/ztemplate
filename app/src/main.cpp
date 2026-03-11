/*
 * Copyright (c) 2026 Igor Petrovic
 * SPDX-License-Identifier: MIT
 */

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
    auto led_toggler_instance = ztemplate::led_toggler::LedToggler(GPIO_DT_SPEC_GET(DT_ALIAS(toggle_led), gpios));

    if (!led_toggler_instance.init())
    {
        LOG_ERR("LED init failed");
        return 0;
    }

    while (1)
    {
        led_toggler_instance.toggle();
        LOG_INF("LED toggled");
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}
