#include "SensorInterface.h"

bool SensorInterface::init() {
    // ---- Try BME680 ----
    if (bme680.begin(I2C_STANDARD_MODE)) {
        bme680.setOversampling(TemperatureSensor, Oversample16);
        bme680.setOversampling(PressureSensor, Oversample16);
        bme680.setOversampling(HumiditySensor, Oversample16);
        bme680.setIIRFilter(IIR4);
        bme680.setGas(320, 150);
        bmeAvailable = true;
        Serial.println("[SensorInterface] BME680 detected");
    } else {
        bmeAvailable = false;
        Serial.println("[SensorInterface] BME680 NOT detected (continuing)");
    }

    // ---- MQ135 (analog sensor) ----
    pinMode(mqPin, INPUT);
    mqAvailable = true;
    Serial.println("[SensorInterface] MQ135 enabled");

    // Init success if at least one sensor works
    return (bmeAvailable || mqAvailable);
}

int8_t SensorInterface::getSensorData(int32_t &temp, int32_t &humidity,
                                      int32_t &pressure, int32_t &gas) {

    // Defaults if sensor missing
    temp = 0;
    humidity = 0;
    pressure = 0;
    gas = 0;

    bool gotSomething = false;

    // ---- Read BME680 if available ----
    if (bmeAvailable) {
        int8_t res = bme680.getSensorData(temp, humidity, pressure, gas);
        if (res >= 0) gotSomething = true;
    }

    // ---- Read MQ135 ----
    // If no BME680, we can still fill "gas"
    if (mqAvailable) {
        int raw = analogRead(mqPin);

        // Store MQ135 raw reading inside "gas"
        // (Since your packet expects gas int32)
        gas = raw;
        gotSomething = true;
    }

    if (gotSomething) return 1;
    return 0;
}
