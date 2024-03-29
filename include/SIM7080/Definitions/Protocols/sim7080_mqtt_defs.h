 /*
 * sim7080_mqtt_defs.h
 *
 *  Copyright (C) Daniel Kampert, 2022
 *	Website: www.kampis-elektroecke.de
 *  File info: SIM70XX driver for ESP32.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Errors and commissions should be reported to DanielKampert@kampis-elektroecke.de.
 */

#ifndef SIM7080_MQTT_DEFS_H_
#define SIM7080_MQTT_DEFS_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

/** @brief SIM7080 MQTT Quality of Service options.
 */
typedef enum
{
    SIM7080_MQTT_QOS_0          = 0,                /**< At most once. */
    SIM7080_MQTT_QOS_1,                             /**< At least one. */
    SIM7080_MQTT_QOS_2,                             /**< Only once. */
} SIM7080_MQTT_QoS_t;

/** @brief SIM7080 MQTT last will object definition.
 */
typedef struct
{
    std::string Topic;                              /**< Last will topic. */
    SIM7080_MQTT_QoS_t QoS;                         /**< Last will quality of service. */
    std::string Message;                            /**< Last will message. */
    bool Retained;                                  /**< Retained flag. */
} SIM7080_MQTT_Will_t;

/** @brief SIM7080 MQTT Socket object.
 */
typedef struct
{
    std::string Broker;                             /**< MQTT broker IP address or domain. */
    uint16_t Port;                                  /**< MQTT port. */
    std::string ClientID;                           /**< MQTT client ID. */
    uint16_t KeepAlive;                             /**< Keep alive interval in seconds. */
    bool CleanSession;                              /**< Clean session flag. */
    bool WillFlag;                                  /**< Last will flag. */
    bool isConnected;                               /**< Socket connected.
                                                         NOTE: Handled by the device driver. */
    bool isCreated;                                 /**< #true when the socket is created.
                                                         NOTE: Handled by the device driver. */
    std::string Username;                           /**< Optional username. */
    std::string Password;                           /**< Optional password. */
    SIM7080_MQTT_QoS_t QoS;                         /**< Quality of service settings. */
    SIM7080_MQTT_Will_t* p_LastWill;                /**< Pointer to last will configuration object. */
} SIM7080_MQTT_Socket_t;

#endif /* SIM7080_MQTT_DEFS_H_ */