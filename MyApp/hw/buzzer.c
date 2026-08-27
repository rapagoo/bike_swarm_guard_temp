#include "buzzer.h"

#include "main.h"

static bool active = false;
static uint32_t stop_at_ms = 0U;

static void buzzer_write(bool on)
{
    HAL_GPIO_WritePin(
        BUZZER_GPIO_Port,
        BUZZER_Pin,
        on ? GPIO_PIN_SET : GPIO_PIN_RESET
    );
}

void buzzer_init(void)
{
    buzzer_write(false);
    active = false;
    stop_at_ms = 0U;
}

void buzzer_beep(uint16_t duration_ms)
{
    if (duration_ms == 0U)
    {
        buzzer_init();
        return;
    }

    buzzer_write(true);
    active = true;
    stop_at_ms = HAL_GetTick() + duration_ms;
}

void buzzer_process(void)
{
    if (active && ((int32_t)(HAL_GetTick() - stop_at_ms) >= 0))
    {
        buzzer_write(false);
        active = false;
    }
}

bool buzzer_is_active(void)
{
    return active;
}
