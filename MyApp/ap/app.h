#ifndef APP_H
#define APP_H

#include "message_type.h"

void app_init(void);
void app_process(void);

/* 현재는 디버거에서 마지막 버튼 메시지를 확인하기 위한 임시 인터페이스입니다. */
message_type_t app_get_last_message(void);

#endif /* APP_H */
