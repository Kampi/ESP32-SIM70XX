 /*
 * sim7080_mqtt.cpp
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

#if((CONFIG_SIMXX_DEV == 7080) && (defined CONFIG_SIM70XX_DRIVER_WITH_MQTT))

#include <esp_log.h>

#include "sim7080.h"
#include "sim7080_mqtt.h"
#include "../../Private/UART/sim70xx_uart.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7080_MQTT";

SIM70XX_Error_t SIM7080_MQTT_Create(SIM7080_t& p_Device, SIM7080_MQTT_Socket_t* p_Socket, std::string Broker, uint16_t Port)
{
    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    p_Socket->KeepAlive = 60;
    p_Socket->Broker = Broker;
    p_Socket->Port = Port;
    p_Socket->ClientID = "SIM7080-MQTT";

    return SIM7080_MQTT_Create(p_Device, p_Socket);
}

SIM70XX_Error_t SIM7080_MQTT_Create(SIM7080_t& p_Device, SIM7080_MQTT_Socket_t* p_Socket)
{
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if((p_Socket == NULL) || (p_Socket->ClientID.size() == 0) || (p_Socket->Broker.size() == 0))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    CommandStr = "AT+SMCONF=\"CLIENTID\",\"" + p_Socket->ClientID + "\"";
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMCONF(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    CommandStr = "AT+SMCONF=\"KEEPTIME\"," + std::to_string(p_Socket->KeepAlive);
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMCONF(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    CommandStr = "AT+SMCONF=\"URL\",\"" + p_Socket->Broker + "\"," + std::to_string(p_Socket->Port);
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMCONF(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    CommandStr = "AT+SMCONF=\"CLEANSS\"," + std::to_string(p_Socket->CleanSession);
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMCONF(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    CommandStr = "AT+SMCONF=\"QOS\"," + std::to_string(p_Socket->QoS);
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMCONF(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    if(p_Socket->p_LastWill != NULL)
    {
        // TODO:
    }

    if(p_Socket->Username.size() > 0)
    {
        CommandStr = "AT+SMCONF=\"USERNAME\"," + p_Socket->Username + "\"";
        SIM70XX_CREATE_CMD(Command);
        *Command = SIM7080_AT_SMCONF(CommandStr);
        SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
        if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
        {
            return SIM70XX_ERR_FAIL;
        }
        SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));
    }

    if(p_Socket->Password.size() > 0)
    {
        CommandStr = "AT+SMCONF=\"PASSWORD\"," + p_Socket->Password + "\"";
        SIM70XX_CREATE_CMD(Command);
        *Command = SIM7080_AT_SMCONF(CommandStr);
        SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
        if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
        {
            return SIM70XX_ERR_FAIL;
        }
        SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));
    }

    // TODO: Add RETAIN
    // TODO: Add SUBHEX
/*
    CommandStr = "AT+SMCONF=\"ASYNCMODE\",0";
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMCONF(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));
*/
    p_Socket->Internal.isCreated = true;

    return SIM70XX_ERR_OK;    
}

SIM70XX_Error_t SIM7080_MQTT_Connect(SIM7080_t& p_Device, SIM7080_MQTT_Socket_t* p_Socket)
{
    SIM70XX_TxCmd_t* Command;

    if((p_Socket == NULL))
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

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMCONN;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    p_Socket->Internal.isConnected = true;
    p_Device.MQTT.Socket = p_Socket;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_MQTT_Publish(SIM7080_t& p_Device, SIM7080_MQTT_Socket_t* p_Socket, std::string Topic, std::string Message, SIM7080_MQTT_QoS_t QoS, bool Retained)
{
    return SIM7080_MQTT_Publish(p_Device, p_Socket, Topic, Message.c_str(), Message.size(), QoS, Retained);
}

SIM70XX_Error_t SIM7080_MQTT_Publish(SIM7080_t& p_Device, SIM7080_MQTT_Socket_t* p_Socket, std::string Topic, const void* p_Buffer, uint16_t Length, SIM7080_MQTT_QoS_t QoS, bool Retained)
{
    std::string Response;
    std::string CommandStr;
    SIM70XX_TxCmd_t Command;

    if((p_Socket == NULL) || (p_Buffer == NULL) || (Topic.size() > 128) || (QoS < SIM7080_MQTT_QOS_0) || (QoS > SIM7080_MQTT_QOS_2) || (Length > 1024))
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

    Command = SIM7080_AT_SMPUB(Topic, Length, QoS, Retained);

    // Transmit the command.
    SIM70XX_UART_SendLine(p_Device.UART, Command.Command);

    // Wait for the empty space after the ">".
    Response = SIM70XX_UART_ReadStringUntil(p_Device.UART, ' ', Command.Timeout * 1000UL);
    if(Response.find(">") == std::string::npos)
    {
        ESP_LOGE(TAG, "Invalid response. Expect '>', got: %s", Response.c_str());

        return SIM70XX_ERR_FAIL;
    }

    // Send the data.
    SIM70XX_UART_Send(p_Device.UART, p_Buffer, Length);

    // Read and parse the status code.
    SIM70XX_UART_ReadStringUntil(p_Device.UART);
    Response = SIM70XX_UART_ReadStringUntil(p_Device.UART, '\n', Command.Timeout * 1000UL);

    ESP_LOGI(TAG, "Response: %s", Response.c_str());

    if(Response.find("OK") != std::string::npos)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_MQTT_Subscribe(SIM7080_t& p_Device, SIM7080_MQTT_Socket_t* p_Socket, std::string Topic, SIM7080_MQTT_QoS_t QoS)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if((p_Socket == NULL) || (Topic.size() > 128) || (QoS < SIM7080_MQTT_QOS_0) || (QoS > SIM7080_MQTT_QOS_2))
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
    *Command = SIM7080_AT_SMSUB(Topic, QoS);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command); 
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    if(p_Socket->Internal.SubQueue == NULL)
    {
        p_Socket->Internal.SubQueue = xQueueCreate(CONFIG_SIM70XX_QUEUE_LENGTH, sizeof(SIM7080_Sub_Evt*));
    }

    p_Socket->Internal.SubTopics++;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_MQTT_GetMessage(SIM7080_MQTT_Socket_t* p_Socket, SIM7080_Sub_Evt* p_Message)
{
    SIM7080_Sub_Evt* Packet;

    if((p_Message == NULL) || (p_Socket->Internal.SubQueue == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(SIM7080_MQTT_MessagesAvailable(p_Socket) == 0)
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

SIM70XX_Error_t SIM7080_MQTT_Unsubscribe(SIM7080_t& p_Device, SIM7080_MQTT_Socket_t* p_Socket, std::string Topic)
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
    *Command = SIM7080_AT_SMUNSUB(Topic);
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

SIM70XX_Error_t SIM7080_MQTT_Disconnect(SIM7080_t& p_Device, SIM7080_MQTT_Socket_t* p_Socket)
{
    SIM70XX_TxCmd_t* Command;

    if((p_Socket == NULL))
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
    else if(p_Socket->Internal.isConnected == false)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMDISC;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    p_Socket->Internal.isConnected = false;
    p_Device.MQTT.Socket = NULL;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_MQTT_Destroy(SIM7080_MQTT_Socket_t* p_Socket)
{
    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    p_Socket->Internal.isCreated = false;

    return SIM70XX_ERR_OK;
}

#endif