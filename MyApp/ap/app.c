#include "app.h"

#include "button.h"

static message_type_t last_message = MSG_NONE;

void app_init(void)
{
    button_init();
}

void app_process(void)
{
    message_type_t message = button_get_message();

    if (message != MSG_NONE)
    {
        /* 다음 단계에서 이 위치에 alert_show(message)를 연결합니다. */
        last_message = message;
    }
}

message_type_t app_get_last_message(void)
{
    return last_message;
}
