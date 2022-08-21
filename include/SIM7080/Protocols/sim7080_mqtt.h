 /*
 * sim7080_mqtt.h
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

#ifndef SIM7080_MQTT_H_
#define SIM7080_MQTT_H_

#include "sim7080_defs.h"
#include "sim70xx_errors.h"
#include "sim7080_mqtt_defs.h"

/** @brief          Create a MQTT socket.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @param Broker   MQTT broker address
 *  @param Port     (Optional) MQTT broker port
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_MQTT_Create(SIM7080_t& p_Device, SIM7080_MQTT_Socket_t* p_Socket, std::string Broker, uint16_t Port = 1883);

/** @brief          Create a MQTT socket.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_MQTT_Create(SIM7080_t& p_Device, SIM7080_MQTT_Socket_t* p_Socket);

/** @brief          Open a connection to a MQTT socket.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_MQTT_Connect(SIM7080_t& p_Device, SIM7080_MQTT_Socket_t* p_Socket);

#endif /* SIM7080_MQTT_H_ */