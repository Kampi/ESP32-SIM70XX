 /*
 * sim7020_mqtt_defs.h
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

#ifndef SIM7020_MQTT_DEFS_H_
#define SIM7020_MQTT_DEFS_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

/** @brief SIM7020 MQTT Quality of Service options.
 */
typedef enum
{
    SIM_MQTT_QOS_0          = 0,                    /**< Quality of service 0. */
    SIM_MQTT_QOS_1,                                 /**< Quality of service 0. */
    SIM_MQTT_QOS_2,                                 /**< Quality of service 0. */
} SIM7020_MQTT_QoS_t;

/** @brief SIM7020 MQTT version options.
 */
typedef enum
{
    SIM_MQTT_31             = 3,                    /**< MQTT version 3.1. */
    SIM_MQTT_311,                                   /**< MQTT version 3.1.1. */
} SIM7020_MQTT_Version_t;

/** @brief SIM7020 MQTT last will object.
 */
typedef struct
{
    std::string Topic;                              /**< Last will topic. */
    SIM7020_MQTT_QoS_t QoS;                         /**< Last will quality of service. */
    std::string Message;                            /**< Last will message. */
    bool Retained;                                  /**< Retained flag. */
} SIM7020_MQTT_Will_t;

/** @brief SIM7020 MQTT publish object.
 */
typedef struct
{
    uint8_t ID;                                     /**< MQTT socket ID. */
    std::string Topic;                              /**< Topic. */
    SIM7020_MQTT_QoS_t QoS;                         /**< Quality of service. */
    bool Retained;                                  /**< Retained flag. */
    bool Dup;                                       /**< Duplicate flag. */
    std::string Payload;                            /**< Message payload. */
} SIM7020_Pub_t;

/** @brief SIM7020 MQTT Socket object.
 */
typedef struct
{
    std::string Broker;                             /**< MQTT broker URL. */
    uint16_t Port;                                  /**< MQTT port. */
    uint8_t CID;                                    /**< PDP context ID. */
    uint16_t Timeout;                               /**< MQTT command timeout in milliseconds.
                                                         NOTE: Possible values are from 0 to 60000. */
    uint32_t BufferSize;                            /**< Buffer size.
                                                         NOTE: Possible values are from 20 to 1132. */
    SIM7020_MQTT_Version_t Version;                 /**< MQTT version. */
    std::string ClientID;                           /**< MQTT client ID. */
    uint16_t KeepAlive;                             /**< Keep alive interval in seconds. */
    bool CleanSession;                              /**< Clean session flag. */
    bool WillFlag;                                  /**< Last will flag. */
    SIM7020_MQTT_Will_t LastWill;                   /**< Last will options.
                                                         NOTE: Only needed when \ref SIM7020_MQTT_Socket_t.WillFlag is set to #true. */
    uint8_t ID;                                     /**< Socket ID from the module.
                                                         NOTE: Handled by the device driver. */
    bool isConnected;                               /**< Socket connected.
                                                         NOTE: Handled by the device driver. */
    bool isCreated;                                 /**< #true when the socket is created.
                                                         NOTE: Handled by the device driver. */
    std::string Username;                           /**< Optional username. */
    std::string Passwort;                           /**< Optional password. */
} SIM7020_MQTT_Socket_t;

#endif /* SIM7020_MQTT_DEFS_H_ */