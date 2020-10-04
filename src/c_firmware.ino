#include "config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <BME280I2C.h>
#include <Wire.h>
#include "wifi.h"

IPAddress ip(DEVICE_IP);
IPAddress gateway(DEVICE_GATEWAY);
IPAddress subnet(255, 255, 255, 0);

BME280I2C bme;
WiFiClient client;
PubSubClient mqtt_client(client);

void setup() {
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);
  delay(100);

  Wire.begin();

  if (!bme.begin()) {
    ESP.deepSleep(SLEEPTIME);
  }
  
  float temp(NAN), pres(NAN), hum(NAN);

  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_bar);
  bme.read(pres, temp, hum, tempUnit, presUnit);
  delay(10);
  bme.read(pres, temp, hum, tempUnit, presUnit);
  pres *= 1e3;

  // Bring up the WiFi connection
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  delay(1);
  wifi_connect();

  mqtt_client.setServer(MQTT_HOSTNAME, MQTT_PORT);
  if (mqtt_client.connect(LOCATION, MQTT_USER, MQTT_PASSWORD)) {
    mqtt_client.publish((std::string("home/") + LOCATION + "/temperature").c_str(), String(temp).c_str());
    mqtt_client.publish((std::string("home/") + LOCATION + "/pressure").c_str(), String(pres).c_str());
    mqtt_client.publish((std::string("home/") + LOCATION + "/humidity").c_str(), String(hum).c_str());
    delay(10);
  }
  digitalWrite(POWER_PIN, LOW);
  WiFi.disconnect(true);
  delay(1);
  ESP.deepSleep(SLEEPTIME);
}


void loop() {
}
