#define DEEP_SLEEP_TIME 1e6

#define STARTUS_LED D8

void setup()
{
    // Setup serial
    Serial.begin(115200);

    // Setup digital inputs/outputs
    pinMode(STARTUS_LED, OUTPUT);
}

void loop()
{
    delay(1000);
    digitalWrite(STARTUS_LED, HIGH);
    delay(1000);
    Serial.println("Hello from loop");

    // Put the ESP in deep sleep
    ESP.deepSleep(DEEP_SLEEP_TIME);
}