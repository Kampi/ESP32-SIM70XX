# ESP32 driver for [SIM70XX] cellular modules.

## Table of Contents

- [ESP32 driver for [SIM70XX] cellular modules.](#esp32-driver-for-sim70xx-cellular-modules)
  - [Table of Contents](#table-of-contents)
  - [About](#about)
  - [Driver status](#driver-status)
    - [Description](#description)
  - [Maintainer](#maintainer)

## About

Multi driver for SIMCom cellular modules with UART interface.

## Driver status

| Driver        | SIM7020       | SIM7080       |
|---------------|---------------|---------------|
| NTP           | Basic         | Basic         |
| DNS           | Advanced      | Advanced      |
| Ping          | Basic         | Basic         |
| GPS           |               | Open          |
| E-Mail        |               | Open          |
| File system   |               | Basic         |
| SSL   		    |               | Open          |
| NVRAM         | Basic         |               |
| TCP (Client)  | Basic         | Basic         |
| UDP (Client)  | Open          | Open          |
| TCP (Server)  | Open          | Open          |
| UDP (Server)  | Open          | Open          |
| HTTP          | Open          | Not started   |
| CoAP          | Open          | Not started   |
| MQTT          | Open          | Open          |
| PSM           | Open          | Not started   |

### Description

**Not started**

Driver implementation has not started.

**Open**

On-going implementation. The driver is not ready to use.

**Basic**

Basic functionallity implemented and tested. Advanced configuration is not implemented.

**Advanced**

Advanced functionallity implemented and tested.

## Maintainer

- [Daniel Kampert](mailto:daniel.kameprt@kampis-elektroecke.de)