#include "config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <BME280I2C.h>
#include <Wire.h>

BME280I2C bme;
WiFiClient client;
PubSubClient mqtt_client(client);

void setup() {
  #ifdef ENABLE_DEBUG
  Serial.begin(115200);
  #endif

  DEBUG_MSG("SETUP: Booted");
  bool config_valid = read_config_auto();

  pinMode(UPDATE_PIN, INPUT);
  if (digitalRead(UPDATE_PIN) == 0 || !config_valid) {
    run_config_server();
  }

  DEBUG_MSG("SETUP: Power on sensor");
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);
  delay(10);

  Wire.begin();

  if (!bme.begin()) {
    DEBUG_MSG("SETUP: Sensor not available");
    ESP.deepSleep(config.sleep_time * 1e6);
  }
  
  float temp(NAN), pres(NAN), hum(NAN);

  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_bar);
  bme.read(pres, temp, hum, tempUnit, presUnit);
  delay(10);
  bme.read(pres, temp, hum, tempUnit, presUnit);
  pres *= 1e3;

  // Bring up the WiFi connection
  DEBUG_MSG("SETUP: Start connecting to Wifi");
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.config(network_ip, network_gateway, network_subnet, network_dns);
  delay(1);
  wifi_connect();
  DEBUG_MSG("SETUP: Done connecting to Wifi");

  float battery_voltage = analogRead(0);
  battery_voltage = 4.9 * battery_voltage / 1023;
  DEBUG_MSG("SETUP: Measured voltage=%f", battery_voltage);

  DEBUG_MSG("SETUP: Start to send MQTT");
  mqtt_client.setServer(config.mqtt_hostname, config.mqtt_port);
  if (mqtt_client.connect(config.sensor_name, config.mqtt_user, config.mqtt_password)) {
    mqtt_client.publish((std::string("home/") + config.sensor_name + "/temperature").c_str(), String(temp).c_str());
    mqtt_client.publish((std::string("home/") + config.sensor_name + "/pressure").c_str(), String(pres).c_str());
    mqtt_client.publish((std::string("home/") + config.sensor_name + "/humidity").c_str(), String(hum).c_str());
    mqtt_client.publish((std::string("home/") + config.sensor_name + "/battery_voltage").c_str(),
        String(battery_voltage).c_str());
    delay(10);
    DEBUG_MSG("SETUP: Sent messages");
  } else {
    DEBUG_MSG("SETUP: MQTT not available");
  }
  digitalWrite(POWER_PIN, LOW);
  WiFi.disconnect(true);
  delay(1);
  DEBUG_MSG("SETUP: Going to sleep");
  ESP.deepSleep(config.sleep_time * 1e6);
}


void loop() {
  ESP.deepSleep(config.sleep_time * 1e6);
}
