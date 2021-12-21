#ifndef SENSORS_H
#define SENSORS_H

#include "multiplexer.h"
#include "analog_sensor.h"

// analog sensors indeces to multiplexer
enum Analog_Sensor_Type
{
    MQ2,
    MQ135,
    IGROMETER,
    PHOTORESITOR,
    RAIN_METER
};

typedef struct Sensors
{
    int temperature;
    int humidity;

    Analog_Sensor_t rain_meter;
    Analog_Sensor_t igrometer;
    Analog_Sensor_t mq2;
    Analog_Sensor_t mq135;
    Analog_Sensor_t ldr;

    int32_t pressure;
    float altitude;
    float bmp_temperature;
} Sensors_t;

void print_sensors(Sensors_t *s)
{
    Serial.println("Values collected from sesnors:");
    Serial.printf("  Temperature: %d˚C\n", s->temperature);
    Serial.printf("  Humidity: %d%\n", s->humidity);
    Serial.printf("  Rain meter: %d (%d%)\n", s->rain_meter.raw, s->rain_meter.percent);
    Serial.printf("  Igrometer: %d (%d%)\n", s->igrometer.raw, s->igrometer.percent);
    Serial.printf("  LDR light: %d (%d%)\n", s->ldr.raw, s->ldr.percent);
    Serial.printf("  MQ-2: %d (%d%)\n", s->mq2.raw, s->mq2.percent);
    Serial.printf("  MQ-135: %d (%d%)\n", s->mq135.raw, s->mq135.percent);
    Serial.printf("  Pressure: %dPa\n", s->pressure);
    Serial.printf("  BMP Temperature: %f˚C\n", s->bmp_temperature);
    Serial.printf("  Altitude: %fmeters\n", s->altitude);
    Serial.println("");
}

#endif // SENSORS_H