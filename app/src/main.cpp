/*
 * Copyright (c) 2026 Igor Petrovic
 * SPDX-License-Identifier: MIT
 */

#include "usbd.h"

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/input/input.h>
#include <zephyr/usb/class/usbd_midi2.h>

#include "zlibs/utils/midi/transport/usb/transport_common.h"
#include "zlibs/utils/midi/transport/usb/transport_usb.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include <ump_stream_responder.h>
#ifdef __cplusplus
}
#endif

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

namespace
{
    LOG_MODULE_REGISTER(main);

    class HwaUsb : public zlibs::utils::midi::usb::Hwa
    {
        public:
        virtual ~HwaUsb() = default;

        virtual bool supported() = 0;
    };

    class HwaUsbHw : public HwaUsb
    {
        public:
        HwaUsbHw() = default;

        bool supported() override
        {
            return true;
        }

        bool init() override
        {
            _instance = this;

            k_fifo_init(&_rx_fifo);

            const auto slab_err = k_mem_slab_init(&_rx_slab, _rx_slab_buffer, sizeof(RxPacket), _rx_packet_count);

            if (slab_err != 0)
            {
                LOG_ERR("Failed to initialize MIDI RX slab (%d)", slab_err);
                return false;
            }

            if (!device_is_ready(_device))
            {
                LOG_ERR("MIDI device not ready");
                return false;
            }

            _responder_cfg = UMP_STREAM_RESPONDER(_device, usbd_midi_send, &_midi_ump_ep_dt);

            _ops.rx_packet_cb = []([[maybe_unused]] const struct device* dev, const struct midi_ump ump)
            {
                if (_instance != nullptr)
                {
                    _instance->on_midi_packet(ump);
                }
            };

            _ops.ready_cb = []([[maybe_unused]] const struct device* dev, const bool ready)
            {
                if (_instance != nullptr)
                {
                    _instance->on_device_ready(ready);
                }
            };

            auto context = usbd_init_device();

            if (context == NULL)
            {
                LOG_ERR("Failed to initialize USB device");
                return false;
            }

            usbd_midi_set_ops(_device, &_ops);

            if (usbd_enable(context))
            {
                LOG_ERR("Failed to enable device support");
                return false;
            }

            return true;
        }

        bool deinit() override
        {
            return true;    // never deinit usb interface, just pretend here
        }

        bool write(const midi_ump& packet) override
        {
            const auto err = usbd_midi_send(_device, packet);

            if (err != 0)
            {
                LOG_WRN("Failed to send MIDI packet (%d)", err);
            }

            return err == 0;
        }

        std::optional<midi_ump> read() override
        {
            auto packet = static_cast<RxPacket*>(k_fifo_get(&_rx_fifo, K_FOREVER));

            if (packet == nullptr)
            {
                return {};
            }

            const auto ump = packet->ump;
            k_mem_slab_free(&_rx_slab, packet);

            return ump;
        }

        private:
        struct RxPacket
        {
            void*    fifo_reserved;
            midi_ump ump;
        };

        static constexpr size_t  _rx_packet_count                                      = 16;
        static inline HwaUsbHw*  _instance                                             = nullptr;
        const device* const      _device                                               = DEVICE_DT_GET(DT_NODELABEL(usb_midi));
        usbd_midi_ops            _ops                                                  = {};
        ump_stream_responder_cfg _responder_cfg                                        = {};
        k_fifo                   _rx_fifo                                              = {};
        k_mem_slab               _rx_slab                                              = {};
        alignas(RxPacket) uint8_t _rx_slab_buffer[_rx_packet_count * sizeof(RxPacket)] = {};
        bool                                     _sysex_tx_active                      = false;
        static inline const ump_endpoint_dt_spec _midi_ump_ep_dt                       = UMP_ENDPOINT_DT_SPEC_GET(DT_NODELABEL(usb_midi));

        void on_midi_packet(const midi_ump ump)
        {
            LOG_INF("Received %s packet (MT=%X)",
                    UMP_MT(ump) == UMP_MT_UMP_STREAM ? "UMP Stream" : "MIDI",
                    UMP_MT(ump));

            switch (UMP_MT(ump))
            {
            case UMP_MT_MIDI1_CHANNEL_VOICE:
            case UMP_MT_DATA_64:
            {
                auto packet = allocate_rx_packet();

                if (packet == nullptr)
                {
                    LOG_WRN("Failed to queue MIDI packet: RX pool exhausted");
                    break;
                }

                packet->ump = ump;
                k_fifo_put(&_rx_fifo, packet);
            }
            break;

            case UMP_MT_UMP_STREAM:
            {
                ump_stream_respond(&_responder_cfg, ump);
            }
            break;

            default:
                break;
            }
        }

        void on_device_ready(const bool ready)
        {
            if (ready)
            {
                LOG_INF("Device ready!");
            }
        }

        RxPacket* allocate_rx_packet()
        {
            void* memory = nullptr;
            return (k_mem_slab_alloc(&_rx_slab, &memory, K_NO_WAIT) == 0) ? static_cast<RxPacket*>(memory) : nullptr;
        }
    };

    HwaUsbHw                     _hwa_usb;
    zlibs::utils::midi::usb::Usb _usb_midi(_hwa_usb);
}    // namespace

int main()
{
    if (!_usb_midi.init())
    {
        return -1;
    }

    LOG_INF("USB device support enabled");

    while (1)
    {
        _usb_midi.read();
    }

    return 0;
}
