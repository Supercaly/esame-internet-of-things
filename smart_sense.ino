#include "multiplexer.h"

#include <DHT.h>

// deep sleep time
#define DEEP_SLEEP_TIME 5e6

// status led
#define STATUS_LED D8
#define TMP_PIN D7

// multiplexer input signals
#define MPX_EN D6
#define MPX_AIN A0
#define MPX_BIT0 D3
#define MPX_BIT1 D5
#define MPX_BIT2 D4

// analog sensors indeces to multiplexer
enum Analog_Sensor_Type
{
    MQ135,
    MQ2,
    IGROMETER,
    PHOTORESITOR,
    RAIN_METER
};

Multiplexer multipexer(MPX_EN, MPX_AIN, MPX_BIT0, MPX_BIT1, MPX_BIT2);
DHT dht(TMP_PIN, DHT11);

void setup()
{
    Serial.begin(115200);
    multipexer.begin();

    pinMode(STATUS_LED, OUTPUT);

    Serial.println("ESP8266 booted!");
}

void loop()
{
    digitalWrite(STATUS_LED, HIGH);
    delay(500);
    Serial.println("Hello from loop");

    // Read rain meter and igrometer
    int rain_value = multipexer.read_analog(RAIN_METER);
    int igrometer_value = multipexer.read_analog(IGROMETER);
    // Read gas sensors MQ2 and MQ135
    int mq2_value = multipexer.read_analog(MQ2);
    int mq135_value = multipexer.read_analog(MQ135);
    // Read ldr
    int light_value = multipexer.read_analog(PHOTORESITOR);
    int light_value_percent = map(light_value, 0, 1023, 0, 100);
    // Read temperature and humidity
    int temp_value = dht.readTemperature();
    int hum_value = dht.readHumidity();
    // Read pressure
    int pressure_value = 0;

    Serial.println("Values collected from sesnors");
    Serial.printf("Temperature: %d˚C\n", temp_value);
    Serial.printf("Humidity: %d%\n", hum_value);
    Serial.printf("Rain meter: %d\n", rain_value);
    Serial.printf("Igrometer: %d\n", igrometer_value);
    Serial.printf("LDR light: %d (%d%)\n", light_value, light_value_percent);
    Serial.printf("MQ-2: %d\n", mq2_value);
    Serial.printf("MQ-135: %d\n", mq135_value);
    Serial.printf("Pressure: %d\n", pressure_value);

    // Put the ESP in deep sleep
    ESP.deepSleep(DEEP_SLEEP_TIME);
}