 /*
 * sim7020_server.cpp
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

#include "sim7020_server.h"
#include "../../../../../Core/Queue/sim70xx_queue.h"
#include "../../../../../Core/Commands/sim70xx_commands.h"

#include "../../../../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7020_TCPIP_Server";

SIM70XX_Error_t SIM7020_Server_CreateSocket(SIM7020_t& p_Device, SIM7020_TCP_Type_t Type, std::string IP, uint16_t Port, SIM7020_TCPIP_Socket_t* p_Socket, SIM7020_TCP_Domain_t Domain, SIM7020_TCP_Protocol_t Protocol, uint8_t CID)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    p_Socket->IP = IP;
    p_Socket->Port = Port;
    p_Socket->Internal.Type = Type;
    p_Socket->Internal.CID = CID;
    p_Socket->Domain = Domain;
    p_Socket->Protocol = Protocol;

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CSOC(p_Socket->Domain, Type, p_Socket->Protocol, p_Socket->Internal.CID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, p_Socket->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    p_Socket->Internal.ID = static_cast<uint8_t>(SIM70XX_Tools_StringToUnsigned(Response));
    p_Device.TCP.Sockets.push_back(p_Socket);
    p_Socket->Internal.isListening = false;
    p_Socket->Internal.isCreated = true;
    p_Socket->Internal.isServer = true;

    SIM70XX_LOGD(TAG, "Socket %u created...", p_Socket->Internal.ID);

    return SIM70XX_ERR_OK;  
}

SIM70XX_Error_t SIM7020_Server_BindSocket(SIM7020_t& p_Device, SIM7020_TCPIP_Socket_t* p_Socket)
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
        return SIM70XX_ERR_SOCKET_NOT_CREATED;
    }
    else if(p_Socket->Internal.isListening == true)
    {
        return SIM70XX_ERR_OK;
    }
    else if(p_Socket->Internal.isServer == false)
    {
        return SIM70XX_ERR_INVALID_SOCKET;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CSOB(p_Socket->Internal.ID, p_Socket->Port, p_Socket->IP);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, p_Socket->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_Server_Listen(SIM7020_t& p_Device, SIM7020_TCPIP_Socket_t* p_Socket)
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
        return SIM70XX_ERR_SOCKET_NOT_CREATED;
    }
    else if(p_Socket->Internal.isListening == true)
    {
        return SIM70XX_ERR_OK;
    }
    else if(p_Socket->Internal.isServer == false)
    {
        return SIM70XX_ERR_INVALID_SOCKET;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CSOLIS(p_Socket->Internal.ID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, p_Socket->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_Server_DestroySocket(SIM7020_t& p_Device, SIM7020_TCPIP_Socket_t* p_Socket)
{
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
        return SIM70XX_ERR_SOCKET_NOT_CREATED;
    }
    else if(p_Socket->Internal.isServer == false)
    {
        return SIM70XX_ERR_INVALID_SOCKET;
    }

    for(std::vector<SIM7020_TCPIP_Socket_t*>::iterator it = p_Device.TCP.Sockets.begin(); it != p_Device.TCP.Sockets.end(); ++it)
    {
        if((*it)->Internal.ID == p_Socket->Internal.ID)
        {
            SIM70XX_LOGI(TAG, "Delete socket: %u", p_Socket->Internal.ID);

            p_Device.TCP.Sockets.erase(it);
            break;
        }
    }

    p_Socket->Internal.isCreated = false;

    return SIM70XX_ERR_OK;
}

#endif