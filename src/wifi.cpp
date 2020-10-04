#include "config.h"
#include "wifi.h"

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

void wifi_connect() {
  // Try to read WiFi settings from RTC memory
  RTCData rtc_data;
  bool rtc_valid = false;
  if (ESP.rtcUserMemoryRead(0, (uint32_t*)&rtc_data, sizeof(rtc_data))) {
    uint32_t crc = calculate_crc32(((uint8_t*)&rtc_data) + 4, sizeof(rtc_data) - 4);
    if (crc == rtc_data.crc32) {
      rtc_valid = true;
    }
  }

  // Try to conect to wifi
  if (rtc_valid) {
    WiFi.begin(WLAN_SSID, WLAN_PASSWORD, rtc_data.channel, rtc_data.bssid, true);
  }
  else {
    // The RTC data was not valid, so make a regular connection
    WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
  }

  int retries = 0;
  int wifi_status = WiFi.status();
  while (wifi_status != WL_CONNECTED) {
    retries++;
    if (retries == 100) {
      // Quick connect is not working, reset WiFi and try regular connection
      WiFi.disconnect();
      delay(10);
      WiFi.forceSleepBegin();
      delay(10);
      WiFi.forceSleepWake();
      delay(10);
      WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
    }
    if (retries == 600) {
      // Giving up after 30 seconds and going back to sleep
      WiFi.disconnect(true);
      delay(1);
      WiFi.mode(WIFI_OFF);
      ESP.deepSleep(SLEEPTIME, WAKE_RF_DISABLED);
      return;
    }
    delay(100);
    wifi_status = WiFi.status();
  }

  // Write current connection info back to RTC
  rtc_data.channel = WiFi.channel();
  
  // Copy 6 bytes of BSSID (AP's MAC address)
  memcpy(rtc_data.bssid, WiFi.BSSID(), 6);
  rtc_data.crc32 = calculate_crc32(((uint8_t*)&rtc_data) + 4, sizeof(rtc_data) - 4);
  ESP.rtcUserMemoryWrite(0, (uint32_t*)&rtc_data, sizeof(rtc_data));
}
