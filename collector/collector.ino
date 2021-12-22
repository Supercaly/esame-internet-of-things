#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <multiplexer.h>
#include <sensors.h>
#include <status_led.h>
#include <constants.h>

// deep sleep time
#define DEEP_SLEEP_TIME_MIN 15
#define DEEP_SLEEP_TIME_US DEEP_SLEEP_TIME_MIN * 60 * 1e6

Sensors sensors;
Status_Led led;
InfluxDBClient influxdb_client(INFLUXDB_URL,
                               INFLUXDB_ORG,
                               INFLUXDB_BUCKET,
                               INFLUXDB_TOKEN,
                               InfluxDbCloud2CACert);
Point influxdb_sensors(INFLUXDB_MEAS);

void setup()
{
    Serial.begin(115200);

    // Init Status Led
    led.begin();
    
    // Init sensors
    if (!sensors.begin())
    {
        Serial.println("ERROR: Could not initialize sensors!!!");
        while (true)
        {
            led.blink(5);
            delay(5000);
        }
    }

    // Init WiFi
    WiFi.mode(WIFI_STA);

    // Init influxdb sensors
    influxdb_sensors.addTag("host", "SWS-01");
    influxdb_sensors.addTag("location", "orto-casa");

    Serial.println("ESP8266 booted!");
}

void loop()
{
    // Try to connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PWD);
    while (WiFi.status() != WL_CONNECTED)
    {
        // Blink led while connecting to WiFi
        led.blink();
        Serial.print(".");
    }

    Serial.printf("\nWiFi connected to \"%s\"\n\n", WIFI_SSID);

    // Read sensors
    sensors.read();
    sensors.print();
    
    // Write sensors to InfluxDb
    write_to_influxdb(&sensors);

    // Put the ESP in deep sleep
    ESP.deepSleep(DEEP_SLEEP_TIME_US);
}

// Write the data from all sensors to InfluxDB
void write_to_influxdb(Sensors *s)
{
    influxdb_sensors.clearFields();

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

    Serial.print("Writing data to InfluxDb... ");
    if (influxdb_client.writePoint(influxdb_sensors))
    {
        Serial.println("Done");
        led.blink();
    }
    else
    {
        Serial.printf("Write failed: %s\n", influxdb_client.getLastErrorMessage());
        led.blink(3);
    }
}