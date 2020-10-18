#ifndef _SENSORS_H_
#define _SENSORS_H_

#include <Zanshin_BME680.h>
#include <BME280I2C.h>
#include <PubSubClient.h>

/**
 * Base class for all sensors defining the interface.
 */
class SensorBase {
  public:
    virtual bool init() = 0;
    virtual void measure() = 0;
    virtual void publish(PubSubClient &mqtt_client, std::string topic_base) = 0;
    bool is_initialized = false;
};

/**
 * Wrapper for measuring the battery voltage.
 */
class BatterySensor : public SensorBase {
  public:
    bool init();
    void measure();
    void publish(PubSubClient &mqtt_client, std::string topic_base);

  protected:
    float battery_voltage_;
};

/**
 * Wrapper class for the BME280.
 */
class BME280Sensor : public SensorBase {
  public:
    bool init();
    void measure();
    void publish(PubSubClient &mqtt_client, std::string topic_base);

  protected:
    float temperature_;
    float humidity_;
    float pressure_;
    BME280I2C bme_;
};

/**
 * Wrapper class for the BME680.
 */
class BME680Sensor : public SensorBase {
  public:
    bool init();
    void measure();
    void publish(PubSubClient &mqtt_client, std::string topic_base);

  protected:
    float temperature_;
    float humidity_;
    float pressure_;
    float gas_;
    BME680_Class bme_;
};

/**
 * Implementation for the TMP100 (simple, so no external library needed)
 */
class TMP100Sensor : public SensorBase {
  public:
    bool init();
    void measure();
    void publish(PubSubClient &mqtt_client, std::string topic_base);

  protected:
    float temperature_;
};

#endif
