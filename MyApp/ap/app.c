#include "app.h"

#include "button.h"
#include "message_service.h"
#include "uart_service.h"
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
volatile HAL_StatusTypeDef uart_debug_status = HAL_ERROR;
volatile uint32_t uart_debug_tx_count = 0U;
volatile uint32_t uart_debug_rx_count = 0U;
volatile uint32_t uart_debug_invalid_count = 0U;
volatile uint32_t uart_debug_dropped_count = 0U;
volatile message_type_t uart_debug_last_received = MSG_NONE;

void app_init(
    SPI_HandleTypeDef *vs1003b_spi,
    UART_HandleTypeDef *message_uart
)
{
    button_init();
    uart_debug_status = uart_service_init(message_uart);

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

    message_service_init(vs1003b_debug_status);
}

void app_process(void)
{
    message_type_t message = button_get_message();

    if (message != MSG_NONE)
    {
        last_message = message;
        message_service_handle(message);
        uart_debug_status = uart_service_send_message(message);
    }

    message_type_t received_message = MSG_NONE;
    if (uart_service_get_message(&received_message))
    {
        uart_debug_last_received = received_message;
        last_message = received_message;
        message_service_handle(received_message);
    }

    message_service_process();

    const message_service_status_t *status = message_service_get_status();
    vs1003b_debug_status = status->audio_status;
    vs1003b_debug_audio_playing = status->audio_playing;
    vs1003b_debug_audio_position = status->audio_position;
    buzzer_debug_active = status->buzzer_active;

    uart_debug_status = uart_service_get_status();
    uart_debug_tx_count = uart_service_get_tx_count();
    uart_debug_rx_count = uart_service_get_rx_count();
    uart_debug_invalid_count = uart_service_get_invalid_count();
    uart_debug_dropped_count = uart_service_get_dropped_count();
}

message_type_t app_get_last_message(void)
{
    return last_message;
}
