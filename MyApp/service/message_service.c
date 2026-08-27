#include "message_service.h"

#include "alert.h"
#include "audio_service.h"
#include "buzzer.h"

#define SAFETY_REMINDER_BEEP_MS 150U

static message_service_status_t service_status = {
    .audio_status = VS1003B_STATUS_INVALID_ARGUMENT,
    .audio_playing = false,
    .audio_position = 0U,
    .buzzer_active = false,
};

void message_service_init(vs1003b_status_t initial_audio_status)
{
    alert_init();
    buzzer_init();

    service_status.audio_status = initial_audio_status;
    service_status.audio_playing = audio_service_is_playing();
    service_status.audio_position = audio_service_position();
    service_status.buzzer_active = buzzer_is_active();
}

void message_service_handle(message_type_t message)
{
    if (message == MSG_NONE)
    {
        return;
    }

    /* 오디오 상태와 관계없이 LED와 부저 기능은 계속 동작합니다. */
    alert_show(message);

    if (message == MSG_SAFETY_REMINDER)
    {
        buzzer_beep(SAFETY_REMINDER_BEEP_MS);
    }

    if (service_status.audio_status == VS1003B_STATUS_OK)
    {
        service_status.audio_status = audio_service_play(message);
    }
}

void message_service_process(void)
{
    if (service_status.audio_status == VS1003B_STATUS_OK)
    {
        service_status.audio_status = audio_service_process();
    }

    buzzer_process();

    service_status.audio_playing = audio_service_is_playing();
    service_status.audio_position = audio_service_position();
    service_status.buzzer_active = buzzer_is_active();
}

const message_service_status_t *message_service_get_status(void)
{
    return &service_status;
}
