#include "openhd_led.h"

#include <chrono>
#include <thread>
#include <functional>

#include "openhd_platform.h"
#include "openhd_util_filesystem.h"

namespace openhd {

    LEDManager& LEDManager::instance() {
        static LEDManager instance{};
        return instance;
    }

    void LEDManager::set_led_status(bool on, const std::function<void(bool)>& toggle_led_fn) {
        toggle_led_fn(on);
    }

    void LEDManager::set_aux_led_status(int status) {
        bool on = status != STATUS_ON;
        if (OHDPlatform::instance().is_rpi()) {
            set_led_status(on, rpi::toggle_secondary_led);
        } else if (OHDPlatform::instance().is_zero3w()) {
            set_led_status(on, zero3w::toggle_secondary_led);
        } else if (OHDPlatform::instance().is_radxa_cm3()) {
            set_led_status(on, radxacm3::toggle_secondary_led);
        }
    }

    void LEDManager::set_rgb_led_status(int status, int color) {
        // Color codes would be handled here if needed.
    }

    void LEDManager::set_secondary_led_status(int status) {
        bool on = status != STATUS_ON;
        if (OHDPlatform::instance().is_rpi()) {
            set_led_status(on, rpi::toggle_secondary_led);
        } else if (OHDPlatform::instance().is_zero3w()) {
            set_led_status(on, zero3w::toggle_secondary_led);
        } else if (OHDPlatform::instance().is_radxa_cm3()) {
            set_led_status(on, radxacm3::toggle_secondary_led);
        }
    }

    void LEDManager::set_primary_led_status(int status) {
        bool on = status != STATUS_ON;
        if (OHDPlatform::instance().is_rpi()) {
            set_led_status(on, rpi::toggle_primary_led);
        } else if (OHDPlatform::instance().is_zero3w()) {
            set_led_status(on, zero3w::toggle_primary_led);
        } else if (OHDPlatform::instance().is_radxa_cm3()) {
            set_led_status(on, radxacm3::toggle_primary_led);
        }
    }

    void LEDManager::blink_led(bool primary, std::chrono::milliseconds on_duration, std::chrono::milliseconds off_duration, int blink_count) {
        auto toggle_fn = primary
            ? (OHDPlatform::instance().is_rpi() ? rpi::toggle_primary_led
            : (OHDPlatform::instance().is_zero3w() ? zero3w::toggle_primary_led
            : radxacm3::toggle_primary_led))
            : (OHDPlatform::instance().is_rpi() ? rpi::toggle_secondary_led
            : (OHDPlatform::instance().is_zero3w() ? zero3w::toggle_secondary_led
            : radxacm3::toggle_secondary_led));
        
        for (int i = 0; i < blink_count; ++i) {
            toggle_fn(true);
            std::this_thread::sleep_for(on_duration);
            toggle_fn(false);
            std::this_thread::sleep_for(off_duration);
        }
    }

    void LEDManager::set_status_loading() {
        // Keep secondary LED on
        set_secondary_led_status(STATUS_ON);

        // SOS Pattern: ... --- ...
        // Short flash = 250 ms on + 250 ms off
        // Long flash = 750 ms on + 250 ms off
        std::chrono::milliseconds short_flash_on(250);
        std::chrono::milliseconds short_flash_off(250);
        std::chrono::milliseconds long_flash_on(750);
        std::chrono::milliseconds long_flash_off(250);

        // SOS Pattern
        for (int i = 0; i < 3; ++i) { // 3 short flashes
            blink_led(true, short_flash_on, short_flash_off, 1);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(750)); // Pause between letters
        for (int i = 0; i < 3; ++i) { // 3 long flashes
            blink_led(true, long_flash_on, long_flash_off, 1);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(750)); // Pause between letters
        for (int i = 0; i < 3; ++i) { // 3 short flashes
            blink_led(true, short_flash_on, short_flash_off, 1);
        }

        // Optionally, you can include a longer pause after the SOS pattern
        // std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    void LEDManager::set_status_error() {
        set_primary_led_status(STATUS_ON);
        set_secondary_led_status(STATUS_ON);
        m_has_error = true;
    }

    LEDManager::LEDManager() : m_has_error(false) {
    }

    LEDManager::~LEDManager() {
    }

    void LEDManager::set_status_okay() {
        // Define and use your base directory and folders as needed
    }
}
