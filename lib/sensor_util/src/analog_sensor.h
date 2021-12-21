#ifndef ANALOG_SENSOR_H
#define ANALOG_SENSOR_H

// Represent the value of an analog sensor
typedef struct Analog_Sensor
{
    int raw;
    int percent;
} Analog_Sensor_t;

// Analog sensors indeces to multiplexer
enum Analog_Sensor_Type
{
    MQ2,
    MQ135,
    IGROMETER,
    PHOTORESITOR,
    RAIN_METER
};

#endif // ANALOG_SENSOR_H