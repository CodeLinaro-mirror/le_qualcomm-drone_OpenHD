#ifndef OPENHD_LED_H
#define OPENHD_LED_H

#include <chrono>
#include <functional>

namespace openhd {

    class LEDManager {
    public:
        static LEDManager& instance();

        void set_status_loading();
        void set_status_error();
        void set_status_okay();
        void set_primary_led_status(int status);
        void set_secondary_led_status(int status);

    private:
        LEDManager();
        ~LEDManager();
        
        void blink_led(bool primary, std::chrono::milliseconds on_duration, std::chrono::milliseconds off_duration, int blink_count);
        
        void set_led_status(bool on, const std::function<void(bool)>& toggle_led_fn);
        void set_aux_led_status(int status);
        void set_rgb_led_status(int status, int color);

        bool m_has_error;
    };

}

#endif // OPENHD_LED_H
