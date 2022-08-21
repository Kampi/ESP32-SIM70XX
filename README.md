# ESP32 driver for [SIM70XX] cellular modules.

## Table of Contents

- [About](#about)
- [Driver status](#driver-status)
  - [Description](#description)
- [History](#history)

## About

Multi driver for SIMCom cellular modules with UART interface.

## Driver status

| Driver        | SIM7020       | SIM7080       | 
|---------------|---------------|---------------|
| NTP           | Basic         | Basic         |
| DNS           | Basic         | Basic         |
| Ping          | Basic         | Basic         |
| GPS           |               | Not started   |
| E-Mail        |               | Open          |
| File system   |               | Basic         |
| SSL   		    |               | Open          |
| NVRAM         | Basic         |               |
| TCP (Client)  | Open          | Basic         |
| UDP (Client)  | Open          | Open          |
| TCP (Server)  | Open          | Open          |
| UDP (Server)  | Open          | Open          |
| HTTP          | Open          | Not started   |
| CoAP          | Open          | Not started   |
| MQTT          | Open          | Not started   |
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