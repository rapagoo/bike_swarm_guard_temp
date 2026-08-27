#ifndef BUZZER_H
#define BUZZER_H

#include <stdbool.h>
#include <stdint.h>

/* 액티브 부저를 꺼진 상태로 초기화합니다. */
void buzzer_init(void);

/* 지정한 시간 동안 부저를 켭니다. 호출 후 buzzer_process()가 계속 실행되어야 합니다. */
void buzzer_beep(uint16_t duration_ms);

/* 경과 시간을 확인해 부저를 자동으로 끕니다. */
void buzzer_process(void);

/* 현재 부저 출력 상태를 반환합니다. */
bool buzzer_is_active(void);

#endif /* BUZZER_H */
