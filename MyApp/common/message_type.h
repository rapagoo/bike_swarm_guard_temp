#ifndef MESSAGE_TYPE_H
#define MESSAGE_TYPE_H

#include <stdint.h>

/*
 * Pico와 STM32가 공통으로 사용하는 메시지 ID입니다.
 * UART로 보낼 때는 message_type_t 변수를 직접 전송하지 않고,
 * uint8_t로 명시적으로 변환하여 1바이트 ID만 전송합니다.
 */
typedef enum
{
    MSG_NONE = 0x00,

    /* 라이더가 버튼으로 보내는 요청 */
    MSG_WATER_REQUEST   = 0x10,
    MSG_FOOD_REQUEST    = 0x11,
    MSG_CAUTION_REQUEST = 0x12,
    MSG_STOP_REQUEST    = 0x13,

    /* 후방 거리 센서에서 발생하는 상태 */
    MSG_REAR_SAFE       = 0x20,
    MSG_REAR_CAUTION    = 0x21,
    MSG_REAR_WARNING    = 0x22,
    MSG_REAR_DANGER     = 0x23,

    /* 낙차 및 사고 관련 메시지 */
    MSG_FALL_DETECTED = 0x30,
    MSG_SOS           = 0x31,

    MSG_UNKNOWN = 0xFF
} message_type_t;

#endif /* MESSAGE_TYPE_H */
