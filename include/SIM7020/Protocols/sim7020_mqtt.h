 /*
 * sim7020_mqtt.h
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

#ifndef SIM7020_MQTT_H_
#define SIM7020_MQTT_H_

#include "sim7020_defs.h"
#include "sim70xx_errors.h"
#include "sim7020_mqtt_defs.h"

/** @brief          Create a MQTT socket.
 *  @param p_Device SIM7020 device object
 *  @param Broker   MQTT broker address
 *  @param Port     MQTT broker port
 *  @param p_Socket Pointer to MQTT socket object
 *  @param CID      (Optional) PDP context ID
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Create(SIM7020_t& p_Device, std::string Broker, uint16_t Port, SIM7020_MQTT_Socket_t* p_Socket, uint8_t CID = 1);

/** @brief          Create a MQTT socket.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Create(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket);

/** @brief          Open a connection to a MQTT socket.
 *  @param p_Device SIM7020 device object
 *  @param Client   Client name
 *  @param p_Socket Pointer to MQTT socket object
 *  @param Version  (Optional) MQTT protocol version
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Connect(SIM7020_t& p_Device, std::string Client, SIM7020_MQTT_Socket_t* p_Socket, SIM7020_MQTT_Version_t Version = SIM7020_MQTT_31);

/** @brief          Open a connection to a MQTT socket.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Connect(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket);

/** @brief          Publish a message over MQTT.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @param Topic    Message topic
 *  @param QoS      Quality of service
 *  @param Message  Message string
 *  @param Retained Retained flag
 *  @param Dup      Duplicated flag
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Publish(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket, std::string Topic, SIM7020_MQTT_QoS_t QoS, std::string Message, bool Retained, bool Dup);

/** @brief          Publish a message over MQTT.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @param Topic    Message topic
 *  @param QoS      Quality of service
 *  @param p_Buffer Pointer to message buffer
 *  @param Length   Buffer length
 *  @param Retained Retained flag
 *  @param Dup      Duplicated flag
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Publish(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket, std::string Topic, SIM7020_MQTT_QoS_t QoS, const void* p_Buffer, uint32_t Length, bool Retained, bool Dup);

/** @brief          Subscribe to a MQTT topic.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @param Topic    Message topic
 *  @param QoS      Quality of service
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Subscribe(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket, std::string Topic, SIM7020_MQTT_QoS_t QoS);

/** @brief              Pop a message from the MQTT subscription queue.
 *                      NOTE: Please call SIM7020_MQTT_Publish first.
 *  @param p_Device     SIM7020 device object
 *  @param p_Message    Pointer to MQTT publish message object
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_GetMessage(SIM7020_t& p_Device, SIM7020_Pub_t* p_Message);

/** @brief          Unsubscribe a MQTT topic.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @param Topic    Message topic
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Unsubscribe(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket, std::string Topic);

/** @brief          Close a MQTT connection and release the socket.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Destroy(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket);

#endif /* SIM7020_MQTT_H_ */