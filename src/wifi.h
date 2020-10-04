#ifndef _WIFI_H_
#define _WIFI_H_

#include <ESP8266WiFi.h>

// RTC memory has a blocksize of 4 bytes, hence pad to something dividable by 4
typedef struct {
  uint32_t crc32;
  uint8_t channel;
  uint8_t bssid[6];
  uint8_t padding;
} RTCData;

uint32_t calculate_crc32(const uint8_t *data, size_t length);
void wifi_connect();

#endif
