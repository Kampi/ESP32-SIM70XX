# ESP32 driver for [SIM70XX](https://www.simcom.com) cellular modules

## Table of Contents

- [ESP32 driver for [SIM70XX] cellular modules](#esp32-driver-for-sim70xx-cellular-modules)
  - [Table of Contents](#table-of-contents)
  - [About](#about)
  - [Driver status](#driver-status)
    - [Description](#description)
      - [Not started](#not-started)
      - [Open](#open)
      - [Basic](#basic)
      - [Complete](#complete)
  - [Examples](#examples)
    - [Using esp-idf](#using-esp-idf)
    - [NTP](#ntp)
      - [Configure the NTP project](#configure-the-ntp-project)
    - [MQTT](#mqtt)
      - [Prepare the broker](#prepare-the-broker)
      - [Configure the MQTT project](#configure-the-mqtt-project)
  - [Maintainer](#maintainer)

## About

Multidevice driver for SIMCom cellular modules with UART interface.

## Driver status

| Driver        | SIM7020       | SIM7080       |
|---------------|---------------|---------------|
| NTP           | Complete      | Complete      |
| DNS           | Complete      | Complete      |
| Ping          | Basic         | Basic         |
| GNSS          |               | Basic         |
| E-Mail        |               | Open          |
| File system   |               | Complete      |
| SSL           |               | Open          |
| NVRAM         | Complete      |               |
| TCP (Client)  | Basic         | Basic         |
| UDP (Client)  | Open          | Open          |
| TCP (Server)  | Open          | Open          |
| UDP (Server)  | Open          | Open          |
| HTTP          | Open          | Not started   |
| CoAP          | Open          | Not started   |
| MQTT          | Open          | Open          |
| PSM           | Open          | Not started   |

### Description

#### Not started

Driver implementation has not started.

#### Open

On-going implementation. The driver is not ready to use.

#### Basic

Basic functionallity is implemented and tested. Advanced configuration is not implemented.

#### Complete

All functionallity is implemented and tested.

## Examples

The component contains an `examples` directory with several examples to test the submodules of the driver.

### Using esp-idf

- Add the submodule to your project by using `git submodule add -b master https://gitlab.server-kampert.de/esp32/libraries/sim70xx.git`
- Append the `Kconfig` content into the `Kconfig` of your project
- Run `make menuconfig` and switch to the `Component config` -> `SIM70XX` menu.
- Select the target device under `Target device` and enable the required submodules under `Drivers`.
- Go back to the `Demo` menu and enable the demos for the selected submodules.
- Run `make` to compile the demo

### NTP

#### Configure the NTP project

- Run `make menuconfig`
- Open the menu `Demo` -> `NTP`
- Select `Enable the SNTP demo`
- Fill in the required informations
- Run `make`
- Execute the demo application
- The demo is fetching the time from the given NTP server
- After fetching the time the demo end

### MQTT

#### Prepare the broker

Download and install [mosquitto](https://mosquitto.org/).

```sh
sudo apt update && sudo apt install -y mosquitto
```

Follow these steps when you want to test the authentication:

- Open the configuration file `/etc/mosquitto/mosquitto.conf`
- Set `per_listener_settings` to `true`
- Set `password_file` to `/etc/mosquitto/pwfile.example`
- Set `acl_file` to `/etc/mosquitto/aclfile.example`

Now run the broker

```sh
mosquitto -c /etc/mosquitto/mosquitto.conf
```

#### Configure the MQTT project

- Run `make menuconfig`
- Open the menu `Demo` -> `MQTT`
- Select `Enable the MQTT demo`
- Fill in the required informations
- Run `make`
- Execute the demo application
- The demo is subscribing and publishing to the given topic
- After receiving a message from the subscription topic the demo ends

## Maintainer

- [Daniel Kampert](mailto:daniel.kameprt@kampis-elektroecke.de)