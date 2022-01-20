#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <multiplexer.h>
#include <sensors.h>
#include <status_led.h>
#include <constants.h>
#include <iot_inferencing.h>

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
    
    // Compute anomaly score
    float anomaly_score;
    if (compute_anomaly(&sensors, &anomaly_score) != 0)
    {
        Serial.println("ERROR: Could not compute anomaly!!!");
        led.blink(5);
    } else 
    {
        // Write sensors to InfluxDb
        write_to_influxdb(&sensors, anomaly_score);
    }

    // Put the ESP in deep sleep
    ESP.deepSleep(DEEP_SLEEP_TIME_US);
}

// Compute the anomaly score from the sensors data
int compute_anomaly(Sensors *s, float *score)
{
    Serial.println("Computing anomaly...");
    
    float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE] = {
        s->altitude,
        s->bmp_temperature,
        s->humidity,
        s->igrometer.percent,
        s->ldr.percent,
        s->mq135.percent,
        s->pressure,
        s->rain_meter.percent,
        s->temperature
    };
    
    signal_t signal;
    int err = numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
    if (err != 0)
    {
        ei_printf("Failed to create signal from features buffer (error: %d)\n", err);
        return 1;
    }
    
    ei_impulse_result_t result = {0};
    err = run_classifier(&signal, &result, false);
    if (err != EI_IMPULSE_OK)
    {
        ei_printf("Failed to run classifier (error: %d)\n", err);
        return 1;
    }
    
    *score = result.anomaly;
    return 0;
}

// Write the anomaly score to InfluxDB
void write_to_influxdb(Sensors *s, float a)
{
    influxdb_sensors.clearFields();

    influxdb_sensors.addField("anomaly_score", a);
    influxdb_sensors.addField("temperature", s->temperature);
    influxdb_sensors.addField("humidity", s->humidity);
    influxdb_sensors.addField("rain_meter_percent", s->rain_meter.percent);
    influxdb_sensors.addField("igrometer_percent", s->igrometer.percent);
    influxdb_sensors.addField("ldr_percent", s->ldr.percent);
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

// Implement printf wrapper used by ML library.
void ei_printf(const char *format, ...)
{
    static char buff[1024] = {0};

    va_list args;
    va_start(args, format);
    int r = vsnprintf(buff, sizeof(buff), format, args);
    va_end(args);

    if (r > 0)
    {
        Serial.write(buff);
    }
}