#include "process_task.h"
#include "../drivers/virtual_sensor.h"
#include "task.h"
#include <stdio.h>

/* Thresholds for warning generation */
#define TEMP_WARN_HIGH      280     /* 28.0 °C */
#define HUMIDITY_WARN_HIGH  750     /* 75.0 %  */

static QueueHandle_t s_sensor_queue;
static QueueHandle_t s_log_queue;

static void process_task(void *params)
{
    (void)params;
    SensorData_t data;
    LogMessage_t  msg;

    for (;;) {
        if (xQueueReceive(s_sensor_queue, &data, pdMS_TO_TICKS(1000)) == pdTRUE) {
            msg.timestamp_ms = data.timestamp_ms;
            msg.level = 0;

            if (data.temperature > TEMP_WARN_HIGH || data.humidity > HUMIDITY_WARN_HIGH)
                msg.level = 1;

            snprintf(msg.text, LOG_MSG_MAX_LEN,
                     "[%5lu ms] TEMP:%3ld.%01ld C  HUM:%3lu.%01lu%%  PRES:%4lu hPa",
                     (unsigned long)data.timestamp_ms,
                     (long)(data.temperature / 10), (long)(data.temperature % 10),
                     (unsigned long)(data.humidity / 10), (unsigned long)(data.humidity % 10),
                     (unsigned long)data.pressure);

            xQueueSend(s_log_queue, &msg, pdMS_TO_TICKS(100));
        }
    }
}

void process_task_create(QueueHandle_t sensor_queue, QueueHandle_t log_queue)
{
    s_sensor_queue = sensor_queue;
    s_log_queue    = log_queue;
    xTaskCreate(process_task, "Process", PROCESS_TASK_STACK_SIZE,
                NULL, PROCESS_TASK_PRIORITY, NULL);
}
