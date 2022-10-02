 /*
 * sim7020_tcp_client.cpp
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

#if((CONFIG_SIMXX_DEV == 7020) && (defined CONFIG_SIM70XX_DRIVER_WITH_TCPIP))

#include <esp_log.h>

#include "sim7020.h"
#include "sim7020_tcpip.h"
#include "Private/sim7020_client.h"
#include "../../../Private/Queue/sim70xx_queue.h"
#include "../../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7020_TCPIP_Client";

SIM70XX_Error_t SIM7020_TCP_Client_Create(SIM7020_t& p_Device, std::string IP, uint16_t Port, SIM7020_TCP_Socket_t* p_Socket, uint16_t Timeout, uint8_t CID, SIM7020_TCP_Domain_t Domain, SIM7020_TCP_Protocol_t Protocol)
{
    return SIM7020_Client_CreateSocket(p_Device, SIM7020_TCP_TYPE_TCP, IP, Port, p_Socket, Timeout, CID, Domain, Protocol);
}

SIM70XX_Error_t SIM7020_TCP_Client_Connect(SIM7020_t& p_Device, SIM7020_TCP_Socket_t* p_Socket)
{
    return SIM7020_Client_ConnectSocket(p_Device, p_Socket);
}

SIM70XX_Error_t SIM7020_TCP_Client_Transmit(SIM7020_t& p_Device, SIM7020_TCP_Socket_t* p_Socket, const void* p_Buffer, uint32_t Length)
{
    uint8_t* Buffer_Temp;
    uint32_t BytesToTransmit;
    SIM70XX_TxCmd_t* Command;

    if((p_Socket == NULL) || (p_Socket->Internal.Type != SIM7020_TCP_TYPE_TCP))
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
        return SIM70XX_ERR_NOT_CONNECTED;
    }

    Buffer_Temp = (uint8_t*)p_Buffer;
    BytesToTransmit = Length;
    do
    {
        uint16_t TransmissionSize;
        std::string Buffer_Hex;

        if(BytesToTransmit > SIM7020_TCP_MAX_PAYLOAD_SIZE)
        {
            TransmissionSize = SIM7020_TCP_MAX_PAYLOAD_SIZE;
        }
        else
        {
            TransmissionSize = BytesToTransmit;
        }

        SIM70XX_Tools_Buf2Hex(Buffer_Temp, TransmissionSize, &Buffer_Hex);

        SIM70XX_CREATE_CMD(Command);
        *Command = SIM7020_AT_CCSOSEND(p_Socket->Internal.ID, Buffer_Hex.size(), Buffer_Hex);
        SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
        if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
        {
            return SIM70XX_ERR_FAIL;
        }
        SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

        if(p_Socket->Internal.isConnected == false)
        {
            return SIM70XX_ERR_NOT_CONNECTED;
        }

        Buffer_Temp += SIM7020_TCP_MAX_PAYLOAD_SIZE;
        BytesToTransmit -= TransmissionSize;
    } while(BytesToTransmit > 0);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_TCP_Client_Receive(SIM7020_t& p_Device, SIM7020_TCP_Socket_t* p_Socket, std::string* p_Buffer)
{
    size_t Index;
    std::string Response;
    SIM70XX_TxCmd_t Command;

    if((p_Socket == NULL) || (p_Buffer == NULL) || (p_Socket->Internal.Type != SIM7020_TCP_TYPE_TCP))
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
    else if(p_Socket->Internal.isDataReceived == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+CSONMI: " + std::to_string(p_Socket->Internal.ID), &Response) == false);

    ESP_LOGD(TAG, "Response: %s", Response.c_str());

    // Get the index of the first delimiter.
    Index = Response.find(",");

    // Get the index of the second delimiter.
    Index = Response.find(",", Index + 1);
    Response = Response.substr(Index + 1);

    SIM70XX_Tools_Hex2ASCII(Response, p_Buffer);

    // Remove the last line end.
    if(p_Buffer->empty() == false)
    {
        p_Buffer->pop_back();
        p_Buffer->pop_back();
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_TCP_Client_Disconnect(SIM7020_t& p_Device, SIM7020_TCP_Socket_t* p_Socket)
{
    return SIM7020_Client_DisconnectSocket(p_Device, p_Socket);
}

SIM70XX_Error_t SIM7020_TCP_Client_Destroy(SIM7020_t& p_Device, SIM7020_TCP_Socket_t* p_Socket)
{
    return SIM7020_Client_DestroySocket(p_Device, p_Socket);
}

#endif