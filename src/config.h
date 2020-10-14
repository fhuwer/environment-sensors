#ifndef _CONFIG_H_
#define _CONFIG_H_ 1

#ifdef ENABLE_DEBUG
#define DEBUG_MSG(format, ...) Serial.printf("%lu: " format "\r\n", micros(), ##__VA_ARGS__)
#else
#define DEBUG_MSG(format, ...)
#endif

#include <ESP8266WiFi.h>

#define CONFIG_SSID "SensorConfig"
#define CONFIG_PASSPHRASE "configuration"

#define UPDATE_PIN 14
#define POWER_PIN 12

extern IPAddress network_ip;
extern IPAddress network_gateway;
extern IPAddress network_dns;
extern IPAddress network_subnet;

/**
 * Structure holding the configuration for the sensor board.
 */
typedef struct {
  uint32_t crc32;

  // Wifi settings
  char wifi_ssid[32];
  char wifi_passphrase[64];

  // Network configuration
  uint32_t network_ip;
  uint32_t network_gateway;
  uint32_t network_dns;

  // MQTT settings
  char mqtt_hostname[64];
  uint32_t mqtt_port;
  char mqtt_user[32];
  char mqtt_password[32];

  // Sensor parameters
  char sensor_name[32];
  uint32_t sleep_time;

  uint8_t channel;
  uint8_t bssid[6];
  uint8_t padding;
} ConfigData_t;

extern ConfigData_t config;
extern bool rtc_config_valid;

uint32_t calculate_crc32(const uint8_t *data, size_t length);

void reset_config_register();
bool read_config_auto();
bool read_config_flash();
bool read_config_rtcmem();

bool write_config_flash();
bool write_config_rtcmem();

void wifi_connect();

void handle_root();
void handle_form();
void run_config_server();

#endif
