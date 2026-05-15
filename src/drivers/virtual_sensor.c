#include "virtual_sensor.h"
#include "FreeRTOS.h"
#include "task.h"

/* Simple LCG pseudo-random generator (no stdlib rand needed) */
static uint32_t s_seed = 42;

static uint32_t lcg_rand(void)
{
    s_seed = s_seed * 1664525UL + 1013904223UL;
    return s_seed;
}

void sensor_init(void)
{
    s_seed = 12345;
}

void sensor_read(SensorData_t *out)
{
    /* Temperature: 20.0 ~ 30.0 °C, represented as 200 ~ 300 */
    out->temperature = 200 + (int32_t)(lcg_rand() % 100);

    /* Humidity: 40.0 ~ 80.0 %, represented as 400 ~ 800 */
    out->humidity = 400 + (lcg_rand() % 400);

    /* Pressure: 1000 ~ 1025 hPa */
    out->pressure = 1000 + (lcg_rand() % 25);

    out->timestamp_ms = xTaskGetTickCount();
}
