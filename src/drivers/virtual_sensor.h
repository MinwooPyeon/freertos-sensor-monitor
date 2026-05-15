#ifndef VIRTUAL_SENSOR_H
#define VIRTUAL_SENSOR_H

#include <stdint.h>

typedef struct {
    int32_t  temperature;   /* 0.1 °C units: 250 = 25.0°C */
    uint32_t humidity;      /* 0.1 % units:  600 = 60.0%  */
    uint32_t pressure;      /* hPa */
    uint32_t timestamp_ms;
} SensorData_t;

void    sensor_init(void);
void    sensor_read(SensorData_t *out);

#endif /* VIRTUAL_SENSOR_H */
