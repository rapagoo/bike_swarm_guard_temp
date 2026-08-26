#include "alert.h"

#include "rgb_led.h"

void alert_init(void)
{
    rgb_led_init();
}

void alert_show(message_type_t message)
{
    switch (message)
    {
        case MSG_WATER_REQUEST:
            rgb_led_set(false, false, true);
            break;

        case MSG_FOOD_REQUEST:
        case MSG_REAR_SAFE:
            rgb_led_set(false, true, false);
            break;

        case MSG_CAUTION_REQUEST:
        case MSG_REAR_CAUTION:
            rgb_led_set(true, true, false);
            break;

        case MSG_STOP_REQUEST:
        case MSG_REAR_WARNING:
        case MSG_REAR_DANGER:
        case MSG_FALL_DETECTED:
        case MSG_SOS:
            rgb_led_set(true, false, false);
            break;

        case MSG_NONE:
        case MSG_UNKNOWN:
        default:
            rgb_led_off();
            break;
    }
}
