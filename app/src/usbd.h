#pragma once

#include <stdint.h>
#include <zephyr/usb/usbd.h>

usbd_context* usbd_init_device();
usbd_context* usbd_setup_device();
