# Environment Sensors
Firmware written in C for ESP8266 based environment sensors for monitoring the home environment.
The boards designed for the sensors will be available in another repository at a later point.

The sensor boards have the option to add different and multiple sensors. Hence, the firmware must
be compiled with the correct flags to enable them. This is done with platformio environments.

## Sensors
The sensors are connected via I²C to the ESP8266. Currently drivers for the following sensors are
available:

- **BMP280**:
  - Sensor from Bosch featuring temperature and pressure.
- **BME280**:
  - Similar to BMP280, but also measures humidity.

## Setup
Install `platformio` and then just run `platformio run` to compile all configurations. To flash it
to a board with a given sensor environment do `platformio run -e ENV -t upload`.

