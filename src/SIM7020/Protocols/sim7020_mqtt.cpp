 /*
 * sim7020_mqtt.cpp
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

#include <sdkconfig.h>

#if((CONFIG_SIMXX_DEV == 7020) && (defined CONFIG_SIM70XX_DRIVER_WITH_MQTT))

#include <esp_log.h>

#include "sim7020.h"
#include "sim7020_mqtt.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7020_MQTT";

SIM70XX_Error_t SIM7020_MQTT_Create(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket, std::string Broker, uint16_t Port, SIM7020_MQTT_Version_t Version, uint8_t CID)
{
    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    p_Socket->Broker = Broker;
    p_Socket->Port = Port;
    p_Socket->Timeout = 12000;
    p_Socket->BufferSize = 1024;
    p_Socket->CID = CID;
    p_Socket->Version = Version;
    p_Socket->ClientID = "SIM7020_MQTT";
    p_Socket->KeepAlive = 600;
    p_Socket->CleanSession = true;
    p_Socket->WillFlag = false;

    return SIM7020_MQTT_Create(p_Device, p_Socket);
}

SIM70XX_Error_t SIM7020_MQTT_Create(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket)
{
    std::string Response;
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if((p_Socket == NULL) || (p_Socket->KeepAlive > 64800) || (p_Socket->Timeout > 60000) || (p_Socket->BufferSize < 20) || (p_Socket->BufferSize > 1132) || (p_Socket->Broker.size() > 50) || (p_Socket->CID > 10) ||
       (p_Socket->Version < SIM7020_MQTT_31) || (p_Socket->Version > SIM7020_MQTT_311) || (p_Socket->ClientID.size() == 0) || (p_Socket->ClientID.size() > 120) ||
       ((p_Socket->WillFlag == true) && ((p_Socket->p_LastWill->Topic.size() == 0) || (p_Socket->p_LastWill->Message.size() == 0))) || ((p_Socket->Username.size() > 100) && (p_Socket->Password.size() > 100)))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    CommandStr = "AT+CMQNEW=\"" + p_Socket->Broker + "\"," + "\"" + std::to_string(p_Socket->Port) + "\"," + std::to_string(p_Socket->Timeout) + "," + std::to_string(p_Socket->BufferSize) + "," + std::to_string(p_Socket->CID);
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CMQNEW(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    p_Socket->Internal.ID = (uint8_t)SIM70XX_Tools_StringToUnsigned(Response);

    // Everything okay. The socket is active now.
    ESP_LOGI(TAG, "Socket %u opened...", p_Socket->Internal.ID);

    p_Socket->Internal.isConnected = false;
    p_Socket->Internal.isCreated = true;
    p_Socket->Internal.SubTopics = 0;

    return SIM70XX_ERR_OK;    
}

SIM70XX_Error_t SIM7020_MQTT_Connect(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket)
{
    std::string Response;
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->Internal.isCreated == false)
    {
        return SIM70XX_ERR_NOT_CREATED;
    }
    else if(p_Socket->Internal.isConnected == true)
    {
        return SIM70XX_ERR_OK;
    }

    CommandStr = "AT+CMQCON=" + std::to_string(p_Socket->Internal.ID) + "," + std::to_string(p_Socket->Version) + ",\"" + p_Socket->ClientID + "\"," + std::to_string(p_Socket->KeepAlive) + "," + std::to_string(p_Socket->CleanSession) + ","  + std::to_string(p_Socket->WillFlag);

    if(p_Socket->WillFlag)
    {
        // TODO
        CommandStr += "";
    }

    if((p_Socket->Username.size() > 0) && (p_Socket->Password.size() > 0))
    {
        CommandStr += ",\"" + p_Socket->Username + "\",\"" + p_Socket->Password + "\"";
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CMQCON(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    p_Socket->Internal.isConnected = true;
    p_Device.MQTT.Sockets.push_back(p_Socket);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_MQTT_Publish(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket, std::string Topic, std::string Message, SIM7020_MQTT_QoS_t QoS, bool Retained, bool Dup)
{
    return SIM7020_MQTT_Publish(p_Device, p_Socket, Topic, Message.c_str(), Message.size(), QoS, Retained, Dup);
}

SIM70XX_Error_t SIM7020_MQTT_Publish(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket, std::string Topic, const void* p_Buffer, uint32_t Length, SIM7020_MQTT_QoS_t QoS, bool Retained, bool Dup)
{
    std::string Buffer_Hex;
    std::string Response;
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if((p_Socket == NULL) || (p_Buffer == NULL) || (Topic.size() > 128) || (QoS < SIM7020_MQTT_QOS_0) || (QoS > SIM7020_MQTT_QOS_2) || (Length < 2) || (Length > 1000))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->Internal.isConnected == false)
    {
        return SIM70XX_ERR_NOT_CONNECTED;
    }

    SIM70XX_Tools_Buf2Hex(p_Buffer, Length, &Buffer_Hex);
    CommandStr = "AT+CMQPUB=" + std::to_string(p_Socket->Internal.ID) + ",\"" + Topic + "\"," + std::to_string(1) + "," + std::to_string(Retained) + "," + std::to_string(Dup) + "," + std::to_string(Length * 2) + ",\"" + Buffer_Hex + "\"";
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CMQCON(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

SIM70XX_Error_t SIM7020_MQTT_Subscribe(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket, std::string Topic, SIM7020_MQTT_QoS_t QoS)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if((p_Socket == NULL) || (Topic.size() > 128) || (QoS < SIM7020_MQTT_QOS_0) || (QoS > SIM7020_MQTT_QOS_2))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->Internal.isConnected == false)
    {
        return SIM70XX_ERR_NOT_CONNECTED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CMQSUB(p_Socket->Internal.ID, Topic, QoS);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    if(p_Socket->Internal.SubQueue == NULL)
    {
        p_Socket->Internal.SubQueue = xQueueCreate(CONFIG_SIM70XX_QUEUE_LENGTH, sizeof(SIM7020_Pub_t*));
    }

    p_Socket->Internal.SubTopics++;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_MQTT_GetMessage(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket, SIM7020_Pub_t* p_Message)
{
    SIM7020_Pub_t* Packet;

    if((p_Message == NULL) || (p_Socket->Internal.SubQueue == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(uxQueueMessagesWaiting(p_Socket->Internal.SubQueue) == 0)
    {
        return SIM70XX_ERR_QUEUE_EMPTY;
    }

    if(xQueueReceive(p_Socket->Internal.SubQueue, &Packet, 0) != pdTRUE)
    {
        return SIM70XX_ERR_FAIL;
    }

    *p_Message = *Packet;

    delete Packet;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_MQTT_Unsubscribe(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket, std::string Topic)
{
    SIM70XX_TxCmd_t* Command;

    if((p_Socket == NULL) || (Topic.size() > 128))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->Internal.isConnected == false)
    {
        return SIM70XX_ERR_NOT_CONNECTED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CMQUNSUB(p_Socket->Internal.ID, Topic);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    if(p_Socket->Internal.SubTopics > 0)
    {
        p_Socket->Internal.SubTopics--;

        // Remove the queue when no subscribed topics left.
        if(p_Socket->Internal.SubTopics == 0)
        {
            if(p_Socket->Internal.SubQueue != NULL)
            {
                vQueueDelete(p_Socket->Internal.SubQueue);
            }
        }
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_MQTT_Disconnect(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->Internal.isCreated == false)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CMQDISCON(p_Socket->Internal.ID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    p_Socket->Internal.SubTopics = 0;
    p_Socket->Internal.isConnected = false;
    if(p_Socket->Internal.SubQueue != NULL)
    {
        vQueueDelete(p_Socket->Internal.SubQueue);
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_MQTT_Destroy(SIM7020_t& p_Device, SIM7020_MQTT_Socket_t* p_Socket)
{
    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    p_Socket->Internal.isCreated = false;

    return SIM70XX_ERR_OK;
}

#endif