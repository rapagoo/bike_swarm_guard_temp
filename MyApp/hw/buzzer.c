#include "buzzer.h"

#include "main.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct
{
    bool on;
    uint32_t duration_ms;
} buzzer_step_t;

typedef struct
{
    const buzzer_step_t *steps;
    size_t step_count;
    bool repeat;
} buzzer_pattern_definition_t;

static const buzzer_step_t short_once_steps[] = {
    {true, 150U},
};

static const buzzer_step_t short_twice_steps[] = {
    {true, 120U},
    {false, 120U},
    {true, 120U},
};

static const buzzer_step_t slow_repeat_steps[] = {
    {true, 150U},
    {false, 850U},
};

static const buzzer_step_t fast_repeat_steps[] = {
    {true, 150U},
    {false, 150U},
};

static buzzer_pattern_t current_pattern = BUZZER_PATTERN_NONE;
static size_t current_step_index = 0U;
static uint32_t current_step_started_at_ms = 0U;

static void buzzer_write(bool on)
{
    HAL_GPIO_WritePin(BUZZER_GPIO_Port,
                      BUZZER_Pin,
                      on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static buzzer_pattern_definition_t buzzer_get_pattern_definition(
    buzzer_pattern_t pattern)
{
    switch (pattern)
    {
        case BUZZER_PATTERN_SHORT_ONCE:
            return (buzzer_pattern_definition_t){
                short_once_steps,
                sizeof(short_once_steps) / sizeof(short_once_steps[0]),
                false};

        case BUZZER_PATTERN_SHORT_TWICE:
            return (buzzer_pattern_definition_t){
                short_twice_steps,
                sizeof(short_twice_steps) / sizeof(short_twice_steps[0]),
                false};

        case BUZZER_PATTERN_SLOW_REPEAT:
            return (buzzer_pattern_definition_t){
                slow_repeat_steps,
                sizeof(slow_repeat_steps) / sizeof(slow_repeat_steps[0]),
                true};

        case BUZZER_PATTERN_FAST_REPEAT:
            return (buzzer_pattern_definition_t){
                fast_repeat_steps,
                sizeof(fast_repeat_steps) / sizeof(fast_repeat_steps[0]),
                true};

        case BUZZER_PATTERN_NONE:
        default:
            return (buzzer_pattern_definition_t){NULL, 0U, false};
    }
}

void buzzer_init(void)
{
    current_pattern = BUZZER_PATTERN_NONE;
    current_step_index = 0U;
    current_step_started_at_ms = HAL_GetTick();
    buzzer_write(false);
}

void buzzer_on(void)
{
    current_pattern = BUZZER_PATTERN_NONE;
    buzzer_write(true);
}

void buzzer_off(void)
{
    current_pattern = BUZZER_PATTERN_NONE;
    buzzer_write(false);
}

void buzzer_play_pattern(buzzer_pattern_t pattern)
{
    buzzer_pattern_definition_t definition =
        buzzer_get_pattern_definition(pattern);

    current_pattern = pattern;
    current_step_index = 0U;
    current_step_started_at_ms = HAL_GetTick();

    if ((definition.steps == NULL) || (definition.step_count == 0U))
    {
        current_pattern = BUZZER_PATTERN_NONE;
        buzzer_write(false);
        return;
    }

    buzzer_write(definition.steps[0].on);
}

void buzzer_update(void)
{
    buzzer_pattern_definition_t definition;
    uint32_t now_ms;

    if (current_pattern == BUZZER_PATTERN_NONE)
    {
        return;
    }

    definition = buzzer_get_pattern_definition(current_pattern);
    now_ms = HAL_GetTick();

    while ((uint32_t)(now_ms - current_step_started_at_ms) >=
           definition.steps[current_step_index].duration_ms)
    {
        current_step_started_at_ms +=
            definition.steps[current_step_index].duration_ms;
        ++current_step_index;

        if (current_step_index >= definition.step_count)
        {
            if (definition.repeat)
            {
                current_step_index = 0U;
            }
            else
            {
                current_pattern = BUZZER_PATTERN_NONE;
                buzzer_write(false);
                return;
            }
        }

        buzzer_write(definition.steps[current_step_index].on);
    }
}
