#ifndef ALERT_H
#define ALERT_H

#include "message_type.h"

/* 경고 출력 장치를 안전한 초기 상태로 만듭니다. */
void alert_init(void);

/* 메시지 의미에 맞는 LED 상태와 부저 패턴을 시작합니다. */
void alert_show(message_type_t message);

/* 진행 중인 논블로킹 경고 패턴을 갱신합니다. */
void alert_update(void);

#endif /* ALERT_H */
