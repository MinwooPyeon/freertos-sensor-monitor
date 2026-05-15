#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "tasks/sensor_task.h"
#include "tasks/process_task.h"
#include "tasks/uart_task.h"
#include "tasks/watchdog_task.h"

static QueueHandle_t     sensor_queue;
static QueueHandle_t     log_queue;
static SemaphoreHandle_t uart_mutex;

void vApplicationMallocFailedHook(void)
{
    for (;;);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;
    for (;;);
}

int main(void)
{
    sensor_queue = xQueueCreate(SENSOR_QUEUE_LENGTH, sizeof(SensorData_t));
    log_queue    = xQueueCreate(LOG_QUEUE_LENGTH,    sizeof(LogMessage_t));
    uart_mutex   = xSemaphoreCreateMutex();

    configASSERT(sensor_queue);
    configASSERT(log_queue);
    configASSERT(uart_mutex);

    sensor_task_create(sensor_queue);
    process_task_create(sensor_queue, log_queue);
    uart_task_create(log_queue, uart_mutex);
    watchdog_task_create(uart_mutex);

    vTaskStartScheduler();

    /* Should never reach here */
    for (;;);
    return 0;
}
