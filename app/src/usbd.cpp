#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/bos.h>

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(usb_config);

namespace
{
    const uint8_t attributes = 0;

    USBD_DEVICE_DEFINE(usb_device,
                       DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)),
                       CONFIG_USB_VID,
                       CONFIG_USB_PID);

    USBD_DESC_LANG_DEFINE(usb_lang);
    USBD_DESC_MANUFACTURER_DEFINE(usb_manufacturer, CONFIG_USB_MANUFACTURER);
    USBD_DESC_PRODUCT_DEFINE(usb_product, CONFIG_USB_PRODUCT);
    IF_ENABLED(CONFIG_HWINFO, (USBD_DESC_SERIAL_NUMBER_DEFINE(usb_serial)));

    USBD_DESC_CONFIG_DEFINE(fs_cfg_desc, "FS Configuration");
    USBD_DESC_CONFIG_DEFINE(hs_cfg_desc, "HS Configuration");

    USBD_CONFIGURATION_DEFINE(usb_fs_config,
                              attributes,
                              CONFIG_USB_MAX_POWER,
                              &fs_cfg_desc);

    USBD_CONFIGURATION_DEFINE(usb_hs_config,
                              attributes,
                              CONFIG_USB_MAX_POWER,
                              &hs_cfg_desc);
}    // namespace

void set_midi2_code_triple(struct usbd_context*  uds_ctx,
                           const enum usbd_speed speed)
{
    /*
     * MIDI2 uses class information from Interface Descriptors and has an
     * Interface Association Descriptor, so advertise the IAD device triple.
     */
    usbd_device_set_code_triple(uds_ctx, speed, USB_BCC_MISCELLANEOUS, 0x02, 0x01);
}

struct usbd_context* usbd_setup_device()
{
    int err;

    err = usbd_add_descriptor(&usb_device, &usb_lang);
    if (err)
    {
        LOG_ERR("Failed to initialize language descriptor (%d)", err);
        return NULL;
    }

    err = usbd_add_descriptor(&usb_device, &usb_manufacturer);
    if (err)
    {
        LOG_ERR("Failed to initialize manufacturer descriptor (%d)", err);
        return NULL;
    }

    err = usbd_add_descriptor(&usb_device, &usb_product);
    if (err)
    {
        LOG_ERR("Failed to initialize product descriptor (%d)", err);
        return NULL;
    }

    IF_ENABLED(CONFIG_HWINFO, (err = usbd_add_descriptor(&usb_device, &usb_serial);))
    if (err)
    {
        LOG_ERR("Failed to initialize SN descriptor (%d)", err);
        return NULL;
    }

    if (USBD_SUPPORTS_HIGH_SPEED &&
        usbd_caps_speed(&usb_device) == USBD_SPEED_HS)
    {
        err = usbd_add_configuration(&usb_device, USBD_SPEED_HS, &usb_hs_config);
        if (err)
        {
            LOG_ERR("Failed to add High-Speed configuration");
            return NULL;
        }

        err = usbd_register_all_classes(&usb_device, USBD_SPEED_HS, 1, NULL);
        if (err)
        {
            LOG_ERR("Failed to add register classes");
            return NULL;
        }

        set_midi2_code_triple(&usb_device, USBD_SPEED_HS);
    }

    err = usbd_add_configuration(&usb_device, USBD_SPEED_FS, &usb_fs_config);
    if (err)
    {
        LOG_ERR("Failed to add Full-Speed configuration");
        return NULL;
    }

    err = usbd_register_all_classes(&usb_device, USBD_SPEED_FS, 1, NULL);
    if (err)
    {
        LOG_ERR("Failed to add register classes");
        return NULL;
    }

    set_midi2_code_triple(&usb_device, USBD_SPEED_FS);
    usbd_self_powered(&usb_device, attributes & USB_SCD_SELF_POWERED);

    return &usb_device;
}

struct usbd_context* usbd_init_device()
{
    int err;

    if (usbd_setup_device() == NULL)
    {
        return NULL;
    }

    err = usbd_init(&usb_device);
    if (err)
    {
        LOG_ERR("Failed to initialize device support");
        return NULL;
    }

    return &usb_device;
}
