#include <PubSubClient.h>
#include "config.h"
#include "sensors.h"

/****************************************************************
 * BATTERY SENSOR
 ****************************************************************/
bool BatterySensor::init() {
  // False as we do not want to have the battery transmitted if no other data is available
  is_initialized = true;
  return false;
}

void BatterySensor::measure() {
  battery_voltage_ = 4.9 * analogRead(0) / 1023; // Voltage divider with R1 = 3.9 and R2 = 1
}

void BatterySensor::publish(PubSubClient &mqtt_client, std::string topic_base) {
  mqtt_client.publish((topic_base + "/battery_voltage").c_str(), String(battery_voltage_).c_str());
}

/****************************************************************
 * BME280
 ****************************************************************/
bool BME280Sensor::init() {
  if (bme_.begin()) {
    is_initialized = true;
  }
  return is_initialized;
}

void BME280Sensor::measure() {
  if (!is_initialized) {
    return;
  }
  BME280::TempUnit temperature_unit(BME280::TempUnit_Celsius);
  BME280::PresUnit pressure_unit(BME280::PresUnit_bar);
  bme_.read(pressure_, temperature_, humidity_, temperature_unit, pressure_unit);
  delay(10);
  bme_.read(pressure_, temperature_, humidity_, temperature_unit, pressure_unit);
  pressure_ *= 1e3;
}

void BME280Sensor::publish(PubSubClient &mqtt_client, std::string topic_base) {
  if (!is_initialized) {
    return;
  }

  mqtt_client.publish((topic_base + "/temperature").c_str(), String(temperature_).c_str());
  mqtt_client.publish((topic_base + "/pressure").c_str(), String(pressure_).c_str());
  mqtt_client.publish((topic_base + "/humidity").c_str(), String(humidity_).c_str());
}

/****************************************************************
 * BME680
 ****************************************************************/
bool BME680Sensor::init() {
  if (bme_.begin()) {
    is_initialized = true;
    bme_.setOversampling(TemperatureSensor, Oversample8);
    bme_.setOversampling(HumiditySensor, Oversample8);
    bme_.setOversampling(PressureSensor, Oversample8);
    bme_.setIIRFilter(IIR4);
  }
  return is_initialized;
}

void BME680Sensor::measure() {
  if (!is_initialized) {
    return;
  }
  int32_t  temp, humidity, pressure, gas;
  bme_.setGas(320, 150);
  bme_.getSensorData(temp, humidity, pressure, gas);
  delay(150);
  bme_.getSensorData(temp, humidity, pressure, gas);
  temperature_ = temp / 100.;
  pressure_ = pressure / 100.;
  humidity_ = humidity / 1000.;
  gas_ = gas / 100.;
}

void BME680Sensor::publish(PubSubClient &mqtt_client, std::string topic_base) {
  if (!is_initialized) {
    return;
  }

  mqtt_client.publish((topic_base + "/temperature").c_str(), String(temperature_).c_str());
  mqtt_client.publish((topic_base + "/pressure").c_str(), String(pressure_).c_str());
  mqtt_client.publish((topic_base + "/humidity").c_str(), String(humidity_).c_str());
  mqtt_client.publish((topic_base + "/gas").c_str(), String(gas_).c_str());
}

/****************************************************************
 * TMP100
 ****************************************************************/
bool TMP100Sensor::init() {
  Wire.begin();
  Wire.beginTransmission(TMP100_I2C_ADDRESS);
  Wire.write(0x01); // CONFIG REGISTER
  Wire.write((1 << 6) | (1 << 5) | (1 << 0)); // Set to power down mode and 12-bit resolution
  Wire.endTransmission();
  is_initialized = true;
  return is_initialized;
}

void TMP100Sensor::measure() {
  if (!is_initialized) {
    return;
  }
  unsigned int data[2] = {0, 0};

  Wire.beginTransmission(TMP100_I2C_ADDRESS);
  Wire.write(0x01); // CONFIG REGISTER
  Wire.write((1 << 7) | (1 << 6) | (1 << 5) | (1 << 0)); // Force measurement
  Wire.endTransmission();
  
  Wire.beginTransmission(TMP100_I2C_ADDRESS);
  Wire.write(0x00); // DATA REGISTER
  Wire.endTransmission();

  Wire.requestFrom(TMP100_I2C_ADDRESS, 2);
  if (Wire.available() == 2) {
    data[0] = Wire.read();
    data[1] = Wire.read();
  }
    
  // Convert the data
  temperature_ = (((data[0] * 256.) + (data[1] & 0xF0)) / 16.) * 0.0625;
}

void TMP100Sensor::publish(PubSubClient &mqtt_client, std::string topic_base) {
  if (!is_initialized) {
    return;
  }

  mqtt_client.publish((topic_base + "/temperature_tmp100").c_str(), String(temperature_, 3).c_str());
}
