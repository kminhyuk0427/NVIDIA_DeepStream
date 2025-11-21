#include "count_manager.h"
#include "mqtt_client.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>

#define MAX_TRACKED 10000

/* 총 카운트와 추적된 ID 저장소 */
static uint64_t total_count = 0;

/* 단순 배열 방식 (원래 방식 유지), 단 동시접근을 막기 위해 mutex 사용 */
static uint64_t seen_ids[MAX_TRACKED];
static int seen_count = 0;

/* mutex로 데이터 보호 */
static pthread_mutex_t cm_lock = PTHREAD_MUTEX_INITIALIZER;

/* overflow 경고를 한 번만 출력하기 위한 플래그 */
static bool overflow_warned = false;

/* 초기화 */
void count_manager_init(void)
{
    pthread_mutex_lock(&cm_lock);
    total_count = 0;
    seen_count = 0;
    overflow_warned = false;
    /* seen_ids 값은 초기화하지 않아도 되지만 명확히 0으로 초기화하려면:
       for (int i = 0; i < MAX_TRACKED; ++i) seen_ids[i] = 0; */
    pthread_mutex_unlock(&cm_lock);
}

/* 내부: 이미 본 ID인지 검사 (mutex 내부에서 호출되도록 설계) */
static bool has_seen_before_locked(uint64_t id)
{
    for (int i = 0; i < seen_count; i++)
    {
        if (seen_ids[i] == id)
            return true;
    }
    return false;
}

/* 내부: ID 기록 (mutex 내부에서 호출되도록 설계) */
static void mark_seen_locked(uint64_t id)
{
    if (seen_count < MAX_TRACKED)
    {
        seen_ids[seen_count++] = id;
    }
    else
    {
        if (!overflow_warned)
        {
            /* 한 번만 경고 출력 */
            fprintf(stderr, "[COUNT] seen_ids overflow: MAX_TRACKED=%d reached. Further IDs will not be recorded.\n", MAX_TRACKED);
            overflow_warned = true;
        }
        /* 더 이상 추가하지 않음 (안전하게 무시) */
    }
}

/* 오브젝트 아이디와 class_id를 받아 처리 (thread-safe) */
void count_manager_process_obj(int class_id, uint64_t object_id)
{
    if (class_id != 1) // 아마도 (0=차, 1=전거, 2=사람, 3=표지판)
        return;
    printf("hello");

    /* critical section: seen/total 보호 */
    pthread_mutex_lock(&cm_lock);

    /* 방어: object_id 값 유효성 검사 (원한다면 임계값 추가) */
    /* 예: if (object_id > SOME_LIMIT) { pthread_mutex_unlock(&cm_lock); return; } */

    if (!has_seen_before_locked(object_id))
    {
        /* 처음 등장한 객체이면 카운트 증가 및 표시 */
        total_count++;
        mark_seen_locked(object_id);

        /* MQTT 전송 (publish 실패/비연결 시 mqtt 구현부가 안전하게 처리함) */
        char msg[64];
        snprintf(msg, sizeof(msg), "%llu", (unsigned long long)total_count);
        /* NOTE: mqtt_client_publish 내부에서 자체적으로 thread-safety를 처리하도록 구현되어 있어야 함.
           (우리가 이미 mqtt_client에 mutex를 도입했음) */
        mqtt_client_publish("deepstream/count", msg);
    }

    pthread_mutex_unlock(&cm_lock);
}

/* 총계 반환 (thread-safe) */
uint64_t count_manager_get_total(void)
{
    pthread_mutex_lock(&cm_lock);
    uint64_t val = total_count;
    pthread_mutex_unlock(&cm_lock);
    return val;
}
