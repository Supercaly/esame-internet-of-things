#include "sensors.h"

Sensors::Sensors() : _dht(DHT_PIN, DHT_TYPE),
                     _mpx(MPX_EN, MPX_AIN, MPX_BIT0, MPX_BIT1, MPX_BIT2) {}
Sensors::~Sensors() {}

bool Sensors::begin()
{
    _mpx.begin();
    _dht.begin();
    bool bmp_status = _bmp.begin();

    return bmp_status;
}

void Sensors::read()
{
    rain_meter = _mpx.read_analog(RAIN_METER);
    igrometer = _mpx.read_analog(IGROMETER);
    mq2 = _mpx.read_analog(MQ2);
    mq135 = _mpx.read_analog(MQ135);
    ldr = _mpx.read_analog(PHOTORESITOR);

    temperature = _dht.readTemperature();
    humidity = _dht.readHumidity();

    pressure = _bmp.readPressure();
    bmp_temperature = _bmp.readTemperature();
    altitude = _bmp.readAltitude();
}

void Sensors::print()
{
    Serial.println("Values collected from sesnors:");
    Serial.printf("  Temperature: %d˚C\n", temperature);
    Serial.printf("  Humidity: %d%\n", humidity);
    Serial.printf("  Rain meter: %d (%d%)\n", rain_meter.raw, rain_meter.percent);
    Serial.printf("  Igrometer: %d (%d%)\n", igrometer.raw, igrometer.percent);
    Serial.printf("  LDR light: %d (%d%)\n", ldr.raw, ldr.percent);
    Serial.printf("  MQ-2: %d (%d%)\n", mq2.raw, mq2.percent);
    Serial.printf("  MQ-135: %d (%d%)\n", mq135.raw, mq135.percent);
    Serial.printf("  Pressure: %dPa\n", pressure);
    Serial.printf("  BMP Temperature: %f˚C\n", bmp_temperature);
    Serial.printf("  Altitude: %fmeters\n", altitude);
    Serial.println("");
}