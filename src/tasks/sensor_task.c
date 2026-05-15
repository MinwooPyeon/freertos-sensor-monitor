#include "sensor_task.h"
#include "../drivers/virtual_sensor.h"
#include "task.h"

#define SENSOR_POLL_MS  500

static QueueHandle_t s_queue;

static void sensor_task(void *params)
{
    (void)params;
    SensorData_t data;
    TickType_t last_wake = xTaskGetTickCount();

    for (;;) {
        sensor_read(&data);
        /* Drop oldest reading if queue full rather than blocking */
        if (xQueueSend(s_queue, &data, 0) == errQUEUE_FULL) {
            SensorData_t discard;
            xQueueReceive(s_queue, &discard, 0);
            xQueueSend(s_queue, &data, 0);
        }
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(SENSOR_POLL_MS));
    }
}

void sensor_task_create(QueueHandle_t sensor_queue)
{
    s_queue = sensor_queue;
    sensor_init();
    xTaskCreate(sensor_task, "Sensor", SENSOR_TASK_STACK_SIZE,
                NULL, SENSOR_TASK_PRIORITY, NULL);
}
