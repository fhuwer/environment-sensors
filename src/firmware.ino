#include "config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "sensors.h"
#include <Wire.h>

WiFiClient client;
PubSubClient mqtt_client(client);
SensorBase *sensors[] = {
#if defined(HAS_BATTERY_SENSOR)
  new BatterySensor(),
#endif
#if defined(HAS_BME280) || defined(HAS_BMP280)
  new BME280Sensor(),
#endif
#if defined(HAS_BME680)
  new BME680Sensor(),
#endif
#if defined(HAS_TMP100)
  new TMP100Sensor(),
#endif
};

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

  bool have_working_sensor = false;
  for (SensorBase *s : sensors) {
    if (s->init()) {
      have_working_sensor = true;
    }
  }
  if (!have_working_sensor) {
    DEBUG_MSG("SETUP: No sensor not available");
    ESP.deepSleep(config.sleep_time * 1e6);
  }
  
  for (SensorBase *s : sensors) {
    s->measure();
  }

  // Bring up the WiFi connection
  DEBUG_MSG("SETUP: Start connecting to Wifi");
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.config(network_ip, network_gateway, network_subnet, network_dns);
  delay(1);
  wifi_connect();
  DEBUG_MSG("SETUP: Done connecting to Wifi");

  DEBUG_MSG("SETUP: Start to send MQTT");
  mqtt_client.setServer(config.mqtt_hostname, config.mqtt_port);
  std::string topic_base = std::string("home/") + config.sensor_name; 
  if (mqtt_client.connect(config.sensor_name, config.mqtt_user, config.mqtt_password)) {
    for (SensorBase *s : sensors) {
      s->publish(mqtt_client, topic_base);
    }
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
