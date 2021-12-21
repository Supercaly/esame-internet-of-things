#include <multiplexer.h>
#include <sensors.h>

#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// deep sleep time
#define DEEP_SLEEP_TIME_SEC 10

// status led
#define STATUS_LED D8
#define TMP_PIN D7

// multiplexer input signals
#define MPX_EN D6
#define MPX_AIN A0
#define MPX_BIT0 D3
#define MPX_BIT1 D5
#define MPX_BIT2 D4

// WiFi Connection
#define WIFI_SSID "MartinRouterKing"
#define WIFI_PWD "mpvgfee4gff"

// Influx db config
#define INFLUXDB_URL "http://93.186.254.118:8086"
#define INFLUXDB_ORG "uniurb"
#define INFLUXDB_BUCKET "test"
#define INFLUXDB_TOKEN "7q44Rz0f0IZYM4SYguqyPB5RPafXPEagZUpRuIUBp3aoDT3HVQzFg5c0Hg_RY8Khk8cH8MjuApdyQsKrFyaF4w=="

Multiplexer multiplexer(MPX_EN, MPX_AIN, MPX_BIT0, MPX_BIT1, MPX_BIT2);
DHT dht(TMP_PIN, DHT11);
Adafruit_BMP085 bmp;

InfluxDBClient influxdb_client(INFLUXDB_URL,
                               INFLUXDB_ORG,
                               INFLUXDB_BUCKET,
                               INFLUXDB_TOKEN,
                               InfluxDbCloud2CACert);
Point influxdb_sensors("data_calisti");

void blink_led(int times)
{
    for (int i = 0; i < times; ++i)
    {
        digitalWrite(STATUS_LED, HIGH);
        delay(500);
        digitalWrite(STATUS_LED, LOW);
        delay(500);
    }
}

void setup()
{
    Serial.begin(115200);

    // Init sensors
    multiplexer.begin();
    dht.begin();
    if (!bmp.begin())
    {
        Serial.println("ERROR: Could not find BMP180 sensor!!!");
        while (true)
        {
            blink_led(5);
            delay(5000);
        }
    }

    pinMode(STATUS_LED, OUTPUT);
    WiFi.mode(WIFI_STA);

    // Init influxdb sensors
    influxdb_sensors.addTag("host", "ESP");
    influxdb_sensors.addTag("location", "MonteCerignone");

    Serial.println("ESP8266 booted!");
}

void loop()
{
    // Try to connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PWD);
    while (WiFi.status() != WL_CONNECTED)
    {
        // Blink led while connecting to WiFi
        blink_led(1);
        Serial.print(".");
    }

    Serial.printf("WiFi connected to %s\n", WIFI_SSID);
    Serial.println("");

    // Read sensors
    Sensors_t sensors;
    read_sensors(&sensors);
    print_sensors(&sensors);

    // Write sensors to InfluxDb
    write_to_influxdb(&sensors);

    // Put the ESP in deep sleep
    ESP.deepSleep(DEEP_SLEEP_TIME_SEC * 1e6);
}

// Read data from all sensors
void read_sensors(Sensors_t *sensors)
{
    sensors->rain_meter = multiplexer.read_analog(RAIN_METER);
    sensors->igrometer = multiplexer.read_analog(IGROMETER);
    sensors->mq2 = multiplexer.read_analog(MQ2);
    sensors->mq135 = multiplexer.read_analog(MQ135);
    sensors->ldr = multiplexer.read_analog(PHOTORESITOR);

    sensors->temperature = dht.readTemperature();
    sensors->humidity = dht.readHumidity();

    sensors->pressure = bmp.readPressure();
    sensors->bmp_temperature = bmp.readTemperature();
    sensors->altitude = bmp.readAltitude();
}

// Write the data from all sensors to InfluxDB
void write_to_influxdb(Sensors_t *s)
{
    influxdb_sensors.clearFields();
    influxdb_sensors.addField("temperature", 0);

    influxdb_sensors.addField("temperature", s->temperature);
    influxdb_sensors.addField("humidity", s->humidity);
    influxdb_sensors.addField("rain_meter_raw", s->rain_meter.raw);
    influxdb_sensors.addField("rain_meter_percent", s->rain_meter.percent);
    influxdb_sensors.addField("igrometer_raw", s->igrometer.raw);
    influxdb_sensors.addField("igrometer_percent", s->igrometer.percent);
    influxdb_sensors.addField("ldr_raw", s->ldr.raw);
    influxdb_sensors.addField("ldr_percent", s->ldr.percent);
    influxdb_sensors.addField("mq2_raw", s->mq2.raw);
    influxdb_sensors.addField("mq2_percent", s->mq2.percent);
    influxdb_sensors.addField("mq135_raw", s->mq135.raw);
    influxdb_sensors.addField("mq135_percent", s->mq135.percent);
    influxdb_sensors.addField("pressure", s->pressure);
    influxdb_sensors.addField("bmp_temperature", s->bmp_temperature);
    influxdb_sensors.addField("altitude", s->altitude);

    Serial.println("Writing data to InfluxDb...");
    if (influxdb_client.writePoint(influxdb_sensors))
    {
        Serial.println("Done");
        blink_led(1);
    }
    else
    {
        Serial.printf("Write failed: %s\n", influxdb_client.getLastErrorMessage());
        blink_led(3);
    }
}