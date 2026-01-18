#ifndef SENSORINTERFACE_H_
#define SENSORINTERFACE_H_

#include <Arduino.h>
#include "BME680.h"
#include "MQ135.h"   // install MQ135 library

/* default pins for I2C interface */
#define DEFAULT_BME680_SDA 41
#define DEFAULT_BME680_SCL 42

/* default pin for MQ135 analog output */
#define DEFAULT_MQ135_PIN 1   // change this based on your board wiring

class SensorInterface {
private:
    uint8_t sda;
    uint8_t scl;
    uint8_t mqPin;

    BME680_Class bme680;
    MQ135 mq135;

    bool bmeAvailable = false;
    bool mqAvailable  = false;

public:
    SensorInterface()
        : sda(DEFAULT_BME680_SDA),
          scl(DEFAULT_BME680_SCL),
          mqPin(DEFAULT_MQ135_PIN),
          mq135(DEFAULT_MQ135_PIN) {}

    SensorInterface(uint8_t sda, uint8_t scl, uint8_t mqPin)
        : sda(sda), scl(scl), mqPin(mqPin), mq135(mqPin) {}

    ~SensorInterface() {}

    bool init();

    // returns 1 if got data, 0 if no sensor, -1 if error
    int8_t getSensorData(int32_t &temp, int32_t &humidity,
                         int32_t &pressure, int32_t &gas);
};

#endif
