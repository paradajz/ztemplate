/*
 * Copyright (c) 2026 Igor Petrovic
 * SPDX-License-Identifier: MIT
 */

#include "tests/tests_common.h"

#include "ztemplate/led_toggler/led_toggler.h"

using namespace ::testing;

namespace
{
    LOG_MODULE_REGISTER(led_toggler_test);
}    // namespace

class LedTogglerTest : public ::testing::Test
{
    protected:
    virtual void SetUp() override
    {
    }

    virtual void TearDown() override
    {
    }

    ztemplate::led_toggler::LedToggler _led_toggler = ztemplate::led_toggler::LedToggler({});
};

TEST_F(LedTogglerTest, Init)
{
    ASSERT_FALSE(_led_toggler.init());
}
