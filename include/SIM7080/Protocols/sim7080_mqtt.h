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

/** @brief          Get the available messages in the subscription queue.
 *  @param p_Socket Pointer to MQTT socket object
 *  @return         Number of messages ready.
 */
uint32_t inline __attribute__((always_inline)) SIM7080_MQTT_MessagesAvailable(SIM7080_MQTT_Socket_t* p_Socket)
{
    if(p_Socket->Internal.isConnected == false)
    {
        return SIM70XX_ERR_NOT_CONNECTED;
    }

    return uxQueueMessagesWaiting(p_Socket->Internal.SubQueue);
}

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

/** @brief          Publish a message over MQTT.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @param Topic    Message topic
 *  @param Message  Message string
 *  @param QoS      (Optional) Quality of Service
 *  @param Retained (Optional) Retained flag
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_MQTT_Publish(SIM7080_t& p_Device, SIM7080_MQTT_Socket_t* p_Socket, std::string Topic, std::string Message, SIM7080_MQTT_QoS_t QoS = SIM7080_MQTT_QOS_0, bool Retained = false);

/** @brief          Publish a message over MQTT.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @param Topic    Message topic
 *  @param p_Buffer Pointer to message buffer
 *  @param Length   Buffer length
 *  @param QoS      (Optional) Quality of Service
 *  @param Retained (Optional) Retained flag
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_MQTT_Publish(SIM7080_t& p_Device, SIM7080_MQTT_Socket_t* p_Socket, std::string Topic, const void* p_Buffer, uint16_t Length, SIM7080_MQTT_QoS_t QoS = SIM7080_MQTT_QOS_0, bool Retained = false);

/** @brief          Subscribe to a MQTT topic.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @param Topic    Message topic
 *  @param QoS      (Optional) Quality of Service
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_MQTT_Subscribe(SIM7080_t& p_Device, SIM7080_MQTT_Socket_t* p_Socket, std::string Topic, SIM7080_MQTT_QoS_t QoS = SIM7080_MQTT_QOS_0);

/** @brief              Pop a message from the MQTT subscription queue.
 *  @param p_Device     SIM7080 device object
 *  @param p_Message    Pointer to MQTT publish message object
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_MQTT_GetMessage(SIM7080_MQTT_Socket_t* p_Socket, SIM7080_Sub_Evt* p_Message);

/** @brief          Unsubscribe a MQTT topic.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @param Topic    Message topic
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_MQTT_Unsubscribe(SIM7080_t& p_Device, SIM7080_MQTT_Socket_t* p_Socket, std::string Topic);

/** @brief          Close a connection to a MQTT socket.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_MQTT_Disconnect(SIM7080_t& p_Device, SIM7080_MQTT_Socket_t* p_Socket);

/** @brief          Destroy a MQTT socket.
 *  @param p_Socket Pointer to MQTT socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_MQTT_Destroy(SIM7080_MQTT_Socket_t* p_Socket);

#endif /* SIM7080_MQTT_H_ */