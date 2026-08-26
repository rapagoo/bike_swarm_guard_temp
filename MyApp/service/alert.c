#include "alert.h"

#include "buzzer.h"
#include "rgb_led.h"

void alert_init(void)
{
    rgb_led_init();
    buzzer_init();
}

void alert_show(message_type_t message)
{
    switch (message)
    {
        case MSG_WATER_REQUEST:
            rgb_led_set(false, false, true);
            buzzer_play_pattern(BUZZER_PATTERN_NONE);
            break;

        case MSG_FOOD_REQUEST:
        case MSG_REAR_SAFE:
            rgb_led_set(false, true, false);
            buzzer_play_pattern(BUZZER_PATTERN_NONE);
            break;

        case MSG_CAUTION_REQUEST:
        case MSG_REAR_CAUTION:
            rgb_led_set(true, true, false);
            buzzer_play_pattern(BUZZER_PATTERN_SHORT_ONCE);
            break;

        case MSG_STOP_REQUEST:
            rgb_led_set(true, false, false);
            buzzer_play_pattern(BUZZER_PATTERN_SHORT_TWICE);
            break;

        case MSG_REAR_WARNING:
            rgb_led_set(true, false, false);
            buzzer_play_pattern(BUZZER_PATTERN_SLOW_REPEAT);
            break;

        case MSG_REAR_DANGER:
        case MSG_FALL_DETECTED:
        case MSG_SOS:
            rgb_led_set(true, false, false);
            buzzer_play_pattern(BUZZER_PATTERN_FAST_REPEAT);
            break;

        case MSG_NONE:
        case MSG_UNKNOWN:
        default:
            rgb_led_off();
            buzzer_play_pattern(BUZZER_PATTERN_NONE);
            break;
    }
}

void alert_update(void)
{
    buzzer_update();
}
