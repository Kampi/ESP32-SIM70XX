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

/** @brief          Get the number subscription messages from the subscription queue.
 *  @param p_Socket Pointer to MQTT socket object
 *  @return         Number of items in the subscription queue.
 */
inline __attribute__((always_inline)) uint32_t SIM7020_MQTT_GetSubcriptionItems(SIM7020_MQTT_Socket_t* const p_Socket)
{
    if((p_Socket == NULL) || (p_Socket->Internal.SubQueue == NULL))
    {
        return 0;
    }

    return uxQueueMessagesWaiting(p_Socket->Internal.SubQueue);
}

/** @brief          Create a MQTT socket by using default settings.
 *  @param p_Device SIM7020 device object
 *  @param p_PDP    Pointer to PDP context
 *  @param Broker   MQTT broker address
 *  @param p_Socket Pointer to MQTT socket object
 *  @param Port     (Optional) MQTT broker port
 *  @param Version  (Optional) MQTT protocol version
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Create(SIM7020_t& p_Device, SIM7020_PDP_Context_t* const p_PDP, SIM7020_MQTT_Socket_t* const p_Socket, std::string Broker, uint16_t Port = 1883, SIM7020_MQTT_Version_t Version = SIM7020_MQTT_311);

/** @brief          Create a MQTT socket.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Create(SIM7020_t& p_Device, SIM7020_PDP_Context_t* const p_PDP, SIM7020_MQTT_Socket_t* const p_Socket);

/** @brief          Open a connection to a MQTT socket.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Connect(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* const p_Socket);

/** @brief          Publish a message over MQTT.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @param Topic    Message topic
 *  @param Message  Message string
 *  @param QoS      (Optional) Quality of service
 *  @param Retained (Optional) Retained flag
 *  @param Dup      (Optional) Duplicated flag
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Publish(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* const p_Socket, std::string Topic, std::string Message, SIM7020_MQTT_QoS_t QoS = SIM7020_MQTT_QOS_0, bool Retained = false, bool Dup = false);

/** @brief          Publish a message over MQTT.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @param Topic    Message topic
 *  @param p_Buffer Pointer to message buffer
 *  @param Length   Buffer length
 *  @param QoS      (Optional) Quality of service
 *  @param Retained (Optional) Retained flag
 *  @param Dup      (Optional) Duplicated flag
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Publish(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* const p_Socket, std::string Topic, const void* const p_Buffer, uint32_t Length, SIM7020_MQTT_QoS_t QoS = SIM7020_MQTT_QOS_0, bool Retained = false, bool Dup = false);

/** @brief          Subscribe to a MQTT topic.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @param Topic    Message topic
 *  @param QoS      (Optional) Quality of service
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Subscribe(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* const p_Socket, std::string Topic, SIM7020_MQTT_QoS_t QoS = SIM7020_MQTT_QOS_0);

/** @brief              Get the next subscription message from the MQTT subscription queue.
 *  @param p_Device     SIM7020 device object
 *  @param p_Socket     Pointer to MQTT socket object
 *  @param p_Message    Pointer to MQTT publish message object
 *  @return             SIM70XX_ERR_OK when successful
 *                      SIM70XX_ERR_QUEUE_EMPTY when no item is available
 */
SIM70XX_Error_t SIM7020_MQTT_GetSubscription(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* const p_Socket, SIM7020_MQTT_Sub_Evt_t* const p_Message);

/** @brief          Unsubscribe from a MQTT topic.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @param Topic    Message topic
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Unsubscribe(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* const p_Socket, std::string Topic);

/** @brief          Close a MQTT socket.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Disconnect(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* const p_Socket);

/** @brief          Release and destroy a socket.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to MQTT socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_MQTT_Destroy(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* const p_Socket);

#endif /* SIM7020_MQTT_H_ */