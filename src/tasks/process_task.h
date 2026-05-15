#ifndef PROCESS_TASK_H
#define PROCESS_TASK_H

#include "FreeRTOS.h"
#include "queue.h"

#define PROCESS_TASK_PRIORITY   2
#define PROCESS_TASK_STACK_SIZE 256
#define LOG_QUEUE_LENGTH        16
#define LOG_MSG_MAX_LEN         80

typedef struct {
    char     text[LOG_MSG_MAX_LEN];
    uint32_t timestamp_ms;
    uint8_t  level;        /* 0=INFO, 1=WARN, 2=ERROR */
} LogMessage_t;

void process_task_create(QueueHandle_t sensor_queue, QueueHandle_t log_queue);

#endif /* PROCESS_TASK_H */
