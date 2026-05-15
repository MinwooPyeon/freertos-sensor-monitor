#include "uart_task.h"
#include "process_task.h"
#include "../drivers/uart_driver.h"
#include "task.h"

static QueueHandle_t     s_log_queue;
static SemaphoreHandle_t s_uart_mutex;

static const char *level_str[] = { "INFO", "WARN", "ERROR" };

static void uart_task(void *params)
{
    (void)params;
    LogMessage_t msg;

    for (;;) {
        if (xQueueReceive(s_log_queue, &msg, portMAX_DELAY) == pdTRUE) {
            if (xSemaphoreTake(s_uart_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                uart_printf("[%s] %s\n", level_str[msg.level], msg.text);
                xSemaphoreGive(s_uart_mutex);
            }
        }
    }
}

void uart_task_create(QueueHandle_t log_queue, SemaphoreHandle_t uart_mutex)
{
    s_log_queue   = log_queue;
    s_uart_mutex  = uart_mutex;
    uart_init();
    xTaskCreate(uart_task, "UART", UART_TASK_STACK_SIZE,
                NULL, UART_TASK_PRIORITY, NULL);
}
