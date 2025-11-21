#ifndef COUNT_MANAGER_H
#define COUNT_MANAGER_H

#include <stdint.h>

/* 카운터 초기화 */
void count_manager_init(void);

/* 객체 처리 (class_id, object_id) */
void count_manager_process_obj(int class_id, uint64_t object_id);

/* 현재 총 카운트 반환 */
uint64_t count_manager_get_total(void);

#endif /* COUNT_MANAGER_H */