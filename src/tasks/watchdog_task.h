#ifndef WATCHDOG_TASK_H
#define WATCHDOG_TASK_H

#include "FreeRTOS.h"
#include "semphr.h"

#define WATCHDOG_TASK_PRIORITY   4
#define WATCHDOG_TASK_STACK_SIZE 128

#define TASK_ID_SENSOR  0
#define TASK_ID_PROCESS 1
#define TASK_ID_UART    2

void watchdog_kick(uint8_t task_id);

void watchdog_task_create(SemaphoreHandle_t uart_mutex);

#endif /* WATCHDOG_TASK_H */
