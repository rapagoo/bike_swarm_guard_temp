#include "app.h"

#include "alert.h"
#include "button.h"

static message_type_t last_message = MSG_NONE;

void app_init(void)
{
    button_init();
    alert_init();
}

void app_process(void)
{
    message_type_t message = button_get_message();

    if (message != MSG_NONE)
    {
        last_message = message;
        alert_show(message);
    }
}

message_type_t app_get_last_message(void)
{
    return last_message;
}
