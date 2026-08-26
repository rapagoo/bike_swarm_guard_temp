#ifndef ALERT_H
#define ALERT_H

#include "message_type.h"

/* 경고 출력 장치를 안전한 초기 상태로 만듭니다. */
void alert_init(void);

/* 메시지 의미에 맞는 LED 상태를 표시합니다. */
void alert_show(message_type_t message);

#endif /* ALERT_H */
