#include "app.h"

#include "alert.h"
#include "audio_service.h"
#include "buzzer.h"
#include "button.h"
#include "vs1003b.h"

#define VS1003B_SCI_CLOCKF_ADDRESS 0x03U
#define VS1003B_SCI_VOL_ADDRESS 0x0BU
#define VS1003B_CLOCKF_3X 0x9800U
#define VS1003B_TEST_VOLUME 0x5050U

static message_type_t last_message = MSG_NONE;

/* 첫 하드웨어 검증 단계에서 디버거 Watch로 확인할 변수입니다. */
volatile vs1003b_status_t vs1003b_debug_status = VS1003B_STATUS_INVALID_ARGUMENT;
volatile uint16_t vs1003b_debug_mode = 0U;
volatile uint16_t vs1003b_debug_clockf = 0U;
volatile uint16_t vs1003b_debug_volume = 0U;
volatile bool vs1003b_debug_audio_playing = false;
volatile uint32_t vs1003b_debug_audio_position = 0U;
volatile bool buzzer_debug_active = false;

void app_init(SPI_HandleTypeDef *vs1003b_spi)
{
    button_init();
    alert_init();
    buzzer_init();

    uint16_t mode = 0U;
    vs1003b_debug_status = vs1003b_init(vs1003b_spi, &mode);
    vs1003b_debug_mode = mode;

    if (vs1003b_debug_status == VS1003B_STATUS_OK)
    {
        vs1003b_debug_status = vs1003b_write_register(
            VS1003B_SCI_CLOCKF_ADDRESS,
            VS1003B_CLOCKF_3X
        );
    }

    if (vs1003b_debug_status == VS1003B_STATUS_OK)
    {
        uint16_t clockf = 0U;
        vs1003b_debug_status = vs1003b_read_register(
            VS1003B_SCI_CLOCKF_ADDRESS,
            &clockf
        );
        vs1003b_debug_clockf = clockf;

        if ((vs1003b_debug_status == VS1003B_STATUS_OK) &&
            (clockf != VS1003B_CLOCKF_3X))
        {
            vs1003b_debug_status = VS1003B_STATUS_REGISTER_MISMATCH;
        }
    }

    if (vs1003b_debug_status == VS1003B_STATUS_OK)
    {
        vs1003b_debug_status = vs1003b_write_register(
            VS1003B_SCI_VOL_ADDRESS,
            VS1003B_TEST_VOLUME
        );
    }

    if (vs1003b_debug_status == VS1003B_STATUS_OK)
    {
        uint16_t volume = 0U;
        vs1003b_debug_status = vs1003b_read_register(
            VS1003B_SCI_VOL_ADDRESS,
            &volume
        );
        vs1003b_debug_volume = volume;

        if ((vs1003b_debug_status == VS1003B_STATUS_OK) &&
            (volume != VS1003B_TEST_VOLUME))
        {
            vs1003b_debug_status = VS1003B_STATUS_REGISTER_MISMATCH;
        }
    }
}

void app_process(void)
{
    message_type_t message = button_get_message();

    if (message != MSG_NONE)
    {
        last_message = message;
        alert_show(message);

        if (message == MSG_SAFETY_REMINDER)
        {
            buzzer_beep(150U);
        }

        if (vs1003b_debug_status == VS1003B_STATUS_OK)
        {
            vs1003b_debug_status = audio_service_play(message);
        }
    }

    if (vs1003b_debug_status == VS1003B_STATUS_OK)
    {
        vs1003b_debug_status = audio_service_process();
    }

    vs1003b_debug_audio_playing = audio_service_is_playing();
    vs1003b_debug_audio_position = audio_service_position();

    buzzer_process();
    buzzer_debug_active = buzzer_is_active();
}

message_type_t app_get_last_message(void)
{
    return last_message;
}
