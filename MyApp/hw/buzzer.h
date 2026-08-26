#ifndef BUZZER_H
#define BUZZER_H

typedef enum
{
    BUZZER_PATTERN_NONE = 0,
    BUZZER_PATTERN_SHORT_ONCE,
    BUZZER_PATTERN_SHORT_TWICE,
    BUZZER_PATTERN_SLOW_REPEAT,
    BUZZER_PATTERN_FAST_REPEAT
} buzzer_pattern_t;

/* CubeMX GPIO 초기화 후 부저를 OFF 상태로 만듭니다. */
void buzzer_init(void);

/* 액티브 부저를 직접 켜거나 끄며, 진행 중인 패턴은 취소합니다. */
void buzzer_on(void);
void buzzer_off(void);

/* 지정한 패턴을 처음부터 시작합니다. */
void buzzer_play_pattern(buzzer_pattern_t pattern);

/* HAL_Delay 없이 패턴의 다음 단계를 진행합니다. 반복문에서 계속 호출합니다. */
void buzzer_update(void);

#endif /* BUZZER_H */
