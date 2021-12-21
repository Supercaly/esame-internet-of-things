#ifndef SENSORS_H
#define SENSORS_H

#include "multiplexer.h"
#include "analog_sensor.h"

#include <DHT.h>
#include <Adafruit_BMP085.h>

// multiplexer input signals
#define MPX_EN D6
#define MPX_AIN A0
#define MPX_BIT0 D3
#define MPX_BIT1 D5
#define MPX_BIT2 D4

// DHT pin and type
#define DHT_PIN D7
#define DHT_TYPE DHT11

class Sensors
{
public:
    Sensors();
    ~Sensors();

    bool begin();

    // Read new data from all sensors
    void read();

    // Print the most recent values of all sensors
    void print();

    // raw values from sensors
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
private:
    // sensors
    Multiplexer _mpx;
    DHT _dht;
    Adafruit_BMP085 _bmp;

};

#endif // SENSORS_H