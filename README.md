# Environment Sensors
Firmware written in C for ESP8266 based environment sensors for monitoring the home environment.
The boards designed for the sensors will be available in another repository at a later point.

The sensor boards have the option to add different and multiple sensors. Hence, the firmware must
be compiled with the correct flags to enable them. This is done with platformio environments.

## Sensors
The sensors are connected via I²C to the ESP8266. Currently drivers for the following sensors are
available:

- **Bosch BMP280**:
  - Sensor from Bosch featuring temperature and pressure.
  - define statement: `HAS_BMP280` (also works with `HAS_BME280`)
- **Bosch BME280**:
  - Similar to BMP280, but also measures humidity.
  - define statement: `HAS_BME280`
- **Bosch BME680**:
  - Similar to BME680, but also measures humidity and gas resistance.
  - define statement: `HAS_BME680`
- **Texas Instruments TMP100**:
  - temperature sensor by Texas Instruments with 12-bit resolution
  - define statement: `HAS_TMP100`
  - to specify address optionally: `TMP100_I2C_ADDRESS` can be specified

## Setup
Install `platformio` and then just run `platformio run` to compile all configurations. To flash it
to a board with a given sensor environment do `platformio run -e ENV -t upload`.

## Board configuration
The sensors can be selected by setting build flags for the environment as done for the ones in the
platformio.ini file.

