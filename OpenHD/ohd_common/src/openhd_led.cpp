#include "openhd_led.h"

#include <chrono>
#include <thread>
#include <utility>
#include <vector>
#include <string>
#include <algorithm>

#include "openhd_platform.h"
#include "openhd_spdlog.h"
#include "openhd_util.h"
#include "openhd_util_filesystem.h"

std::vector<std::string> listLedFoldersWithBrightness(const std::string& baseDir) {
    std::vector<std::string> ledFolders;
    auto entries = OHDFilesystemUtil::getAllEntriesFullPathInDirectory(baseDir);

    for (const auto& entry : entries) {
        std::string brightnessFile = entry + "/brightness";
        if (OHDFilesystemUtil::exists(brightnessFile)) {
            std::string folderName = entry.substr(baseDir.length());
            if (folderName.front() == '/') folderName = folderName.substr(1);
            ledFolders.push_back(folderName);
        }
    }

    return ledFolders;
}

void setLedBrightness(const std::vector<std::string>& ledFolders, const std::string& baseDir, const std::string& value) {
    for (const auto& folder : ledFolders) {
        std::string brightnessFile = baseDir + folder + "/brightness";
        OHDFilesystemUtil::write_file(brightnessFile, value);
    }
}

void turnOffAllLeds(const std::vector<std::string>& ledFolders, const std::string& baseDir) {
    setLedBrightness(ledFolders, baseDir, "0");
}

void turnOnAllLeds(const std::vector<std::string>& ledFolders, const std::string& baseDir) {
    setLedBrightness(ledFolders, baseDir, "1");
}

namespace openhd::rpi {
    void toggle_led(const std::string& filename, bool on) {
        if (!OHDFilesystemUtil::exists(filename)) {
            return;
        }
        OHDFilesystemUtil::write_file(filename, on ? "1" : "0");
    }

    void toggle_secondary_led(bool on) {
        static constexpr auto filename = "/sys/class/leds/PWR/brightness";
        toggle_led(filename, on);
    }

    void toggle_primary_led(bool on) {
        static constexpr auto filename = "/sys/class/leds/ACT/brightness";
        toggle_led(filename, on);
    }

    void toggle_led_delayed(bool on, const std::chrono::milliseconds& delay) {
        toggle_secondary_led(on);
        std::this_thread::sleep_for(delay);
        toggle_secondary_led(!on);
    }
}

namespace openhd::zero3w {
    void toggle_led(const std::string& filename, bool on) {
        OHDFilesystemUtil::write_file(filename, on ? "1" : "0");
    }

    void toggle_secondary_led(bool on) {
        static constexpr auto filename = "/sys/class/leds/mmc0::/brightness";
        toggle_led(filename, on);
    }

    void toggle_primary_led(bool on) {
        static constexpr auto filename = "/sys/class/leds/board-led/brightness";
        toggle_led(filename, on);
    }
}

namespace openhd::radxacm3 {
    void toggle_led(const std::string& filename, bool on) {
        OHDFilesystemUtil::write_file(filename, on ? "1" : "0");
    }

    void toggle_secondary_led(bool on) {
        static constexpr auto filename = "/sys/class/leds/pwr-led-red/brightness";
        toggle_led(filename, on);
    }

    void toggle_primary_led(bool on) {
        static constexpr auto filename = "/sys/class/leds/pi-led-green/brightness";
        toggle_led(filename, on);
    }
}

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
        // List of colors and their corresponding numbers:
        // 0 - Red
        // 1 - Green
        // 2 - Blue
        // 3 - Cyan
        // 4 - Magenta
        // 5 - Yellow
        // 6 - White
        // 7 - Black

        std::string colorName;

        if (color == 0) {
            colorName = "Red";
            r = 1; g = 0; b = 0;
        } else if (color == 1) {
            colorName = "Green";
            r = 0; g = 1; b = 0;
        } else if (color == 2) {
            colorName = "Blue";
            r = 0; g = 0; b = 1;
        } else if (color == 3) {
            colorName = "Cyan";
            r = 0; g = 1; b = 1;
        } else if (color == 4) {
            colorName = "Magenta";
            r = 1; g = 0; b = 1;
        } else if (color == 5) {
            colorName = "Yellow";
            r = 1; g = 1; b = 0;
        } else if (color == 6) {
            colorName = "White";
            r = 1; g = 1; b = 1;
        } else if (color == 7) {
            colorName = "Black";
            r = 0; g = 0; b = 0;
        } else {
            colorName = "Unknown";
        }

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

    LEDManager::LEDManager() {
    }

    LEDManager::~LEDManager() {
    }

    void LEDManager::loop() {
    }

    void LEDManager::set_status_okay() {
        std::string baseDir = "/sys/class/leds/";
        auto folders = listLedFoldersWithBrightness(baseDir);
        turnOnAllLeds(folders, baseDir);
    }
}
