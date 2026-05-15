#include "watchdog_task.h"
#include "../drivers/uart_driver.h"
#include "task.h"

#define MAX_TASKS           3
#define WATCHDOG_PERIOD_MS  2000
#define KICK_TIMEOUT_MS     3000    /* task must kick within this window */

typedef struct {
    uint32_t last_kick_ms;
    uint8_t  alive;
} TaskHB_t;

static TaskHB_t      s_heartbeat[MAX_TASKS];
static SemaphoreHandle_t s_uart_mutex;

void watchdog_kick(uint8_t task_id)
{
    if (task_id < MAX_TASKS) {
        s_heartbeat[task_id].last_kick_ms = xTaskGetTickCount();
        s_heartbeat[task_id].alive = 1;
    }
}

static void watchdog_task(void *params)
{
    (void)params;
    TickType_t last_wake = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(WATCHDOG_PERIOD_MS));

        uint32_t now = xTaskGetTickCount();
        uint8_t any_fault = 0;

        for (uint8_t i = 0; i < MAX_TASKS; i++) {
            uint32_t elapsed = now - s_heartbeat[i].last_kick_ms;
            if (s_heartbeat[i].alive && elapsed > pdMS_TO_TICKS(KICK_TIMEOUT_MS)) {
                any_fault = 1;
                if (xSemaphoreTake(s_uart_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
                    uart_printf("[WATCHDOG] Task %u stalled! elapsed=%lu ms\n",
                                (unsigned)i, (unsigned long)(elapsed * 1000 / configTICK_RATE_HZ));
                    xSemaphoreGive(s_uart_mutex);
                }
            }
        }

        if (!any_fault) {
            if (xSemaphoreTake(s_uart_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
                uart_printf("[WATCHDOG] All tasks healthy at %lu ms\n",
                            (unsigned long)now);
                xSemaphoreGive(s_uart_mutex);
            }
        }
    }
}

void watchdog_task_create(SemaphoreHandle_t uart_mutex)
{
    s_uart_mutex = uart_mutex;
    for (uint8_t i = 0; i < MAX_TASKS; i++) {
        s_heartbeat[i].last_kick_ms = 0;
        s_heartbeat[i].alive        = 0;
    }
    xTaskCreate(watchdog_task, "Watchdog", WATCHDOG_TASK_STACK_SIZE,
                NULL, WATCHDOG_TASK_PRIORITY, NULL);
}
