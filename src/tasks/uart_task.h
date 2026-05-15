#ifndef UART_TASK_H
#define UART_TASK_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#define UART_TASK_PRIORITY   1
#define UART_TASK_STACK_SIZE 256

void uart_task_create(QueueHandle_t log_queue, SemaphoreHandle_t uart_mutex);

#endif /* UART_TASK_H */
