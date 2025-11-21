#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <stdint.h>
#include <stdbool.h>

/* MQTT 초기화 */
void mqtt_client_init(const char *host, int port);

/* 문자열 publish (안전한 내부 체크 포함) */
void mqtt_client_publish(const char *topic, const char *payload);

/* 숫자 publish (uint64) */
void mqtt_client_publish_uint64(const char *topic, uint64_t value);

/* MQTT 종료 처리 */
void mqtt_client_deinit(void);

/* 현재 MQTT 연결 여부 확인 */
bool mqtt_client_is_connected(void);

#endif /* MQTT_CLIENT_H */
