#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include "FreeRTOS.h"
#include "queue.h"

#define SENSOR_TASK_PRIORITY    3
#define SENSOR_TASK_STACK_SIZE  256
#define SENSOR_QUEUE_LENGTH     8

void sensor_task_create(QueueHandle_t sensor_queue);

#endif /* SENSOR_TASK_H */
