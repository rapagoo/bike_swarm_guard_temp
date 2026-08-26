#include "app.h"
#include "button.h"
#include "rgb_led.h"
#include "main.h"

static message_type_t last_message = MSG_NONE;

void app_init(void) {
  button_init();

  rgb_led_set(true, false, false); // 빨강
  HAL_Delay(1000);

  rgb_led_set(false, true, false); // 초록
  HAL_Delay(1000);

  rgb_led_set(false, false, true); // 파랑  
  HAL_Delay(1000);

  rgb_led_set(true, true, false); // 노랑
  HAL_Delay(1000);

  rgb_led_off();
}

void app_process(void) {
  message_type_t message = button_get_message();

  if (message != MSG_NONE) {
    /* 다음 단계에서 이 위치에 alert_show(message)를 연결합니다. */
    last_message = message;
  }
}

message_type_t app_get_last_message(void) { return last_message; }
