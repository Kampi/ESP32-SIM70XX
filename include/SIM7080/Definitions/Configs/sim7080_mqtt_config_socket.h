 /*
 * sim7080_mqtt_config_socket.h
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

#ifndef SIM7080_MQTT_CONFIG_SOCKET_H_
#define SIM7080_MQTT_CONFIG_SOCKET_H_

/** @brief          Declare a default MQTT socket. The socket is defined with the following parameter
 *                      Timeout         12000 ms
 *                      Buffersize      1024 Bytes
 *                      MQTT Version    3.1
 *                      Client ID       SIM7020 MQTT
 *                      Keep Alive      600 s
 *  @param Host     Host address
 *  @param HostPort Port number
 */
#define SIM7080_MQTT_DEFAULT_SOCKET(Host, Port)                     {                                       \
                                                                        .Broker = Host,                     \
                                                                        .Port = HostPort,                   \
                                                                        .ClientID = "SIM7080_MQTT",         \
                                                                        .KeepAlive = 600,                   \
                                                                        .CleanSession = true,               \
                                                                        .p_LastWill = NULL,                 \
                                                                        .Username = "",                     \
                                                                        .Password = "",                     \
                                                                        .Internal = {                       \
                                                                            .isConnected = false,           \
                                                                            .isCreated = false,             \
                                                                            .SubTopics = 0,                 \
                                                                            .SubQueue = NULL,               \
                                                                        }                                   \
                                                                    }
#endif /* SIM7080_MQTT_CONFIG_SOCKET_H_ */