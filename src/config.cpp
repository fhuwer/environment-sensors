#include "config.h"
#include "page.h"
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <LittleFS.h>

bool rtc_config_valid = false;
IPAddress network_ip;
IPAddress network_gateway;
IPAddress network_dns;
IPAddress network_subnet(255, 255, 255, 0);

ConfigData_t config;
DNSServer dns_server;
ESP8266WebServer server(80);

/**
 * Calculate the CRC32 checksum for the given data.
 *
 * @param data Data to calculate CRC for.
 * @param length Length of the data.
 * @return CRC32 checksum of the given data.
 */
uint32_t calculate_crc32(const uint8_t *data, size_t length) {
  uint32_t crc = 0xffffffff;
  while (length--) {
    uint8_t c = *data++;
    for (uint32_t i = 0x80; i > 0; i >>= 1) {
      bool bit = crc & 0x80000000;
      if (c & i) {
        bit = !bit;
      }

      crc <<= 1;
      if (bit) {
        crc ^= 0x04c11db7;
      }
    }
  }
  return crc;
}

/**
 * Reset the config structure to empty/default values. This is used in case the config read is not
 * valid as determined by the checksum check.
 */
void reset_config_register() {
  memcpy(config.wifi_ssid, "", sizeof(config.wifi_ssid));
  memcpy(config.wifi_passphrase, "", sizeof(config.wifi_passphrase));
  memcpy(config.mqtt_hostname, "", sizeof(config.mqtt_hostname));
  memcpy(config.mqtt_user, "", sizeof(config.mqtt_user));
  memcpy(config.mqtt_password, "", sizeof(config.mqtt_password));
  memcpy(config.sensor_name, "", sizeof(config.sensor_name));
  config.mqtt_port = 1883;
  config.sleep_time = 60;
}

/**
 * Read the configuration from flash memory.
 * This can leave and invalid configuration in the config structure, hence, it should normally not
 * be called directly, but the method read_config_auto should be used instead.
 *
 * @return True if a valid configuration was loaded.
 */
bool read_config_flash() {
  DEBUG_MSG("CONFIG: Try reading from flash");
  LittleFS.begin();
  File f = LittleFS.open("config", "r");
  f.read((byte*)&config, sizeof(ConfigData_t));
  f.close();
  LittleFS.end();

  uint32_t crc = calculate_crc32(((uint8_t*)&config) + 4, sizeof(config) - 4);
  return crc == config.crc32;
}

/**
 * Read the configuration from RTC memory.
 * This can leave and invalid configuration in the config structure, hence, it should normally not
 * be called directly, but the method read_config_auto should be used instead.
 *
 * @return True if a valid configuration was loaded.
 */
bool read_config_rtcmem() {
  DEBUG_MSG("CONFIG: Try reading from RTC");
  if (ESP.rtcUserMemoryRead(0, (uint32_t*)&config, sizeof(config))) {
    uint32_t crc = calculate_crc32(((uint8_t*)&config) + 4, sizeof(config) - 4);
    if (crc == config.crc32) {
      return true;
    }
  }
  return false;
}

/**
 * Read the configuration from either RTC memory or flash.
 * It will first try RTC memory and fallback to flash. In case none of the two configurations are
 * valid, the configuration register is reset to the default with reset_config_register.
 *
 * @return True if a valid configuration was loaded.
 */
bool read_config_auto() {
  if (read_config_rtcmem()) {
    DEBUG_MSG("CONFIG: Valid from RTC");
    rtc_config_valid = true;
  } else if (read_config_flash()) {
    DEBUG_MSG("CONFIG: Valid from flash");
  } else {
    DEBUG_MSG("CONFIG: No valid config");
    reset_config_register();
    return false;
  }
  network_ip = config.network_ip;
  network_gateway = config.network_gateway;
  network_dns = config.network_dns;
  return true;
}

/**
 * Write the configuration to flash. The CRC checksum is calculated automatically before storing
 * it.
 */
bool write_config_flash() {
  DEBUG_MSG("CONFIG: Write to flash");
  LittleFS.begin();
  File f = LittleFS.open("config", "w");
  config.crc32 = calculate_crc32(((uint8_t*)&config) + 4, sizeof(config) - 4);
  f.write((byte*)&config, sizeof(ConfigData_t));
  f.close();
  LittleFS.end();
  return true;
}

/**
 * Write the configuration to RTC memory. The CRC checksum is calculated automatically before
 * storing it.
 */
bool write_config_rtcmem() {
  DEBUG_MSG("CONFIG: Write to RTC");
  config.crc32 = calculate_crc32(((uint8_t*)&config) + 4, sizeof(config) - 4);
  ESP.rtcUserMemoryWrite(0, (uint32_t*)&config, sizeof(config));
  return true;
}

/**
 * Connect to Wifi using the credentials from the config structure.
 * If rtc_config_valid is set to true, it will try to connect with the last saved BSSID and
 * channel to prevent a wifi scan.
 */
void wifi_connect() {
  // Try to conect to wifi
  if (rtc_config_valid) {
    DEBUG_MSG("WIFI: Connecting with RTC");
    WiFi.begin(config.wifi_ssid, config.wifi_passphrase, config.channel, config.bssid, true);
  }
  else {
    // The RTC data was not valid, so make a regular connection
    DEBUG_MSG("WIFI: Connecting with flash");
    WiFi.begin(config.wifi_ssid, config.wifi_passphrase);
  }

  int retries = 0;
  int wifi_status = WiFi.status();
  while (wifi_status != WL_CONNECTED) {
    retries++;
    if (retries == 100) {
      // Quick connect is not working, reset WiFi and try regular connection
      DEBUG_MSG("WIFI: Quick connect did not work");
      WiFi.disconnect();
      delay(10);
      WiFi.forceSleepBegin();
      delay(10);
      WiFi.forceSleepWake();
      delay(10);
      WiFi.begin(config.wifi_ssid, config.wifi_passphrase);
    }
    if (retries == 600) {
      // Giving up after 30 seconds and going back to sleep
      DEBUG_MSG("WIFI: Connection failed, going to sleep");
      WiFi.disconnect(true);
      delay(1);
      WiFi.mode(WIFI_OFF);
      ESP.deepSleep(config.sleep_time * 1e6);
      return;
    }
    delay(100);
    wifi_status = WiFi.status();
  }

  // Write current connection info back to RTC
  config.channel = WiFi.channel();
  
  // Copy 6 bytes of BSSID (AP's MAC address)
  memcpy(config.bssid, WiFi.BSSID(), 6);
  write_config_rtcmem();
}

/**
 * Handle a request on the captive portal for configuration. This will provide a page containing
 * the configuration form.
 */
void handle_root() {
  DEBUG_MSG("SERVER: Handling root");
  String content(root_html);
  IPAddress tip(192, 168, 178, 1);
  String tipi((uint32_t)tip);
  content.replace("{{sensor_name}}", config.sensor_name);
  content.replace("{{wifi_ssid}}", config.wifi_ssid);
  if (strcmp(config.wifi_passphrase, "") == 0) {
    content.replace("{{wifi_passphrase_placeholder}}", "");
  } else {
    content.replace("{{wifi_passphrase_placeholder}}", "unchanged");
  }
  content.replace("{{wifi_ssid}}", config.wifi_ssid);
  content.replace("{{network_ip}}", network_ip.toString());
  content.replace("{{network_gateway}}", network_gateway.toString());
  content.replace("{{network_dns}}", network_dns.toString());
  content.replace("{{mqtt_hostname}}", config.mqtt_hostname);
  content.replace("{{mqtt_port}}", String(config.mqtt_port));
  content.replace("{{mqtt_user}}", config.mqtt_user);
  if (strcmp(config.mqtt_password, "") == 0) {
    content.replace("{{mqtt_password_placeholder}}", "");
  } else {
    content.replace("{{mqtt_password_placeholder}}", "unchanged");
  }
  content.replace("{{sleep_time}}", String(config.sleep_time));
  server.send(200, "text/html", content);
}

/**
 * Handle a request for the storage form from the captive portal.
 */
void handle_form() {
  DEBUG_MSG("SERVER: Handling form");
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    for (uint8_t i = 0; i < server.args(); i++) {
      if (server.argName(i) == "reboot") {
        ESP.restart();
      } else if (server.argName(i) == "sensor_name") {
        memcpy(config.sensor_name, server.arg(i).c_str(), sizeof(config.sensor_name));
      } else if (server.argName(i) == "wifi_ssid") {
        memcpy(config.wifi_ssid, server.arg(i).c_str(), sizeof(config.wifi_ssid));
      } else if (server.argName(i) == "wifi_passphrase") {
        if (server.arg(i) != "") {
          memcpy(config.wifi_passphrase, server.arg(i).c_str(), sizeof(config.wifi_passphrase));
        }
      } else if (server.argName(i) == "network_ip") {
        network_ip.fromString(server.arg(i));
        config.network_ip = (uint32_t)network_ip;
      } else if (server.argName(i) == "network_gateway") {
        network_gateway.fromString(server.arg(i));
        config.network_gateway = (uint32_t)network_gateway;
      } else if (server.argName(i) == "network_dns") {
        network_dns.fromString(server.arg(i));
        config.network_dns = (uint32_t)network_dns;
      } else if (server.argName(i) == "mqtt_hostname") {
        memcpy(config.mqtt_hostname, server.arg(i).c_str(), sizeof(config.mqtt_hostname));
      } else if (server.argName(i) == "mqtt_port") {
        config.mqtt_port = server.arg(i).toInt();
      } else if (server.argName(i) == "mqtt_user") {
        memcpy(config.mqtt_user, server.arg(i).c_str(), sizeof(config.mqtt_user));
      } else if (server.argName(i) == "mqtt_password") {
        if (server.arg(i) != "") {
          memcpy(config.mqtt_password, server.arg(i).c_str(), sizeof(config.mqtt_password));
        }
      } else if (server.argName(i) == "sleep_time") {
        config.sleep_time = server.arg(i).toInt();
      }
    }
    write_config_flash();
    write_config_rtcmem();
    handle_root();
  }
}

/**
 * Run the WiFi-AP, the http configuration server and a DNS server with a captive portal.
 */
void run_config_server() {
  DEBUG_MSG("SERVER: Starting server");
  WiFi.softAP(CONFIG_SSID, CONFIG_PASSPHRASE);
  IPAddress ap_ip = WiFi.softAPIP();

  dns_server.setErrorReplyCode(DNSReplyCode::NoError);
  dns_server.start(53, "*", ap_ip);

  server.on("/", handle_root);
  server.on("/post/", handle_form);

  // Captive portal
  server.on("/generate_204", handle_root);
  server.on("/favicon.ico", handle_root);
  server.on("/fwlink", handle_root);

  server.begin();
  while (true) {
    dns_server.processNextRequest();
    server.handleClient();
  }
}
