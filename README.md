# ESP32 driver for [SIM70XX](https://www.simcom.com) cellular modules

## Table of Contents

- [ESP32 driver for SIM70XX cellular modules](#esp32-driver-for-sim70xx-cellular-modules)
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
    - [NVRAM](#nvram)
    - [File System](#file-system)
    - [E-Mail](#e-mail)
    - [MQTT](#mqtt)
      - [Prepare the broker](#prepare-the-broker)
      - [Configure the MQTT project](#configure-the-mqtt-project)
    - [NTP](#ntp)
  - [Maintainer](#maintainer)

## About

Multidevice driver for SIMCom cellular modules with UART interface.

## Driver status

| Driver        | SIM7020       | SIM7080       |
|---------------|---------------|---------------|
| NTP           | Complete      | Complete      |
| DNS           | Complete      | Complete      |
| Ping          | Complete      | Complete      |
| GNSS          |               | Basic         |
| E-Mail        |               | Basic         |
| File system   |               | Complete      |
| SSL           |               | Open          |
| NVRAM         | Complete      |               |
| TCP (Client)  | Complete      | Open          |
| UDP (Client)  | Complete      |               |
| TCP (Server)  | Open          |               |
| UDP (Server)  |               |               |
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

### NVRAM

This demo writes and read data from a specific NVRAM key. In the next step all keys are read out and the demo key gets erased from NVRAM.

- Run `make menuconfig`
- Open the menu `Demo` -> `NVRAM`
- Select `Enable the NVRAM demo`
- Run `make`
- Execute the demo application

### File System

This demo is writing data into a file. Then the data from the file is read and the file gets renamed and the original file will be deleted.

- Run `make menuconfig`
- Open the menu `Demo` -> `File System`
- Select `Enable the File System demo`
- Run `make`
- Execute the demo application

### E-Mail

This demo is sending a E-mail to the given recipient. At next all E-Mails from the inbox are read and then deleted.

- Run `make menuconfig`
- Open the menu `Demo` -> `E-Mail`
- Select `Enable the E-Mail demo`
- Fill in the required informations
- Run `make`
- Execute the demo application

> **Warning**
> Please make sure that the inbox doesn´t contain any important E-Mails!
> Depending on your e-mail service you have to enable SSL too!

### MQTT

This demo open a connection to a given MQTT broker. The demo will subscribe and publish data to the given topics. After receiving a message from the subscripted topic the demo ends.

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


### NTP

This demo demo is fetching the time from the given NTP server. After fetching the time the demo ends.

- Run `make menuconfig`
- Open the menu `Demo` -> `NTP`
- Select `Enable the SNTP demo`
- Fill in the required informations
- Run `make`
- Execute the demo application

## Maintainer

- [Daniel Kampert](mailto:daniel.kameprt@kampis-elektroecke.de)