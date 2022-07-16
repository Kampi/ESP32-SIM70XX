 /*
 * sim7020_tcpip.cpp
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

#if((defined CONFIG_SIM70XX_DEV_SIM7020) && (defined CONFIG_SIM70XX_PROT_WITH_TCPIP))

#include <esp_log.h>

#include "sim7020.h"
#include "sim7020_tcpip.h"
#include "../Private/include/sim7020_queue.h"
#include "../Private/include/sim7020_commands.h"

static const char* TAG = "SIM7020_TCPIP";

SIM70XX_Error_t SIM7020_TCP_CreateTCP(const SIM7020_t* const p_Device, std::string IP, uint16_t Port, SIM7020_TCP_Socket_t* p_Socket, uint8_t CID, SIM7020_TCP_Domain_t Domain, SIM7020_TCP_Protocol_t Protocol)
{
    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    p_Socket->IP = IP;
    p_Socket->Port = Port;
    p_Socket->Timeout = 60;
    p_Socket->CID = CID;
    p_Socket->Domain = Domain;
    p_Socket->Type = SIM_TCP_TYPE_TCP;
    p_Socket->Protocol = Protocol;

    return SIM7020_TCP_Create(p_Device, p_Socket);
}

SIM70XX_Error_t SIM7020_TCP_Create(const SIM7020_t* const p_Device, SIM7020_TCP_Socket_t* p_Socket)
{
    std::string Response;
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_Socket == NULL) || (p_Socket->Type == 0) || (p_Socket->Type > SIM_TCP_TYPE_RAW) || (p_Socket->Domain < SIM_TCP_DOMAIN_IPV4) || (p_Socket->Domain > SIM_TCP_DOMAIN_IPV6) || (p_Socket->Protocol < SIM_TCP_PROT_IP) || (p_Socket->Protocol > SIM_TCP_PROT_ICMP) || (p_Socket->CID > 10))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    CommandStr = "AT+CSOC=" + std::to_string(p_Socket->Domain) + "," + std::to_string(p_Socket->Type) + "," + std::to_string(p_Socket->CID);
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CSOC(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, p_Socket->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    p_Socket->ID = (uint8_t)std::stoi(Response);

    // Everything okay. The socket is active now.
    ESP_LOGI(TAG, "Socket %u opened...", p_Socket->ID);

    p_Device->TCP.Sockets.push_back(p_Socket);
    p_Socket->isConnected = false;
    p_Socket->isCreated = true;

    return SIM70XX_ERR_OK;    
}

SIM70XX_Error_t SIM7020_TCP_Connect(const SIM7020_t* const p_Device, SIM7020_TCP_Socket_t* p_Socket)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_Socket == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->isCreated == false)
    {
        return SIM70XX_ERR_NOT_CREATED;
    }
    else if(p_Socket->isConnected == true)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CSOCON(p_Socket->ID, p_Socket->Port, p_Socket->IP);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, p_Socket->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    p_Socket->isConnected = true;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_TCP_TransmitBytes(const SIM7020_t* const p_Device, SIM7020_TCP_Socket_t* p_Socket, const void* p_Buffer, uint16_t Length)
{
    std::string Buffer_Hex;
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_Socket == NULL) || (Length > 512) || (p_Socket->Type != SIM_TCP_TYPE_TCP))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->isConnected == false)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_Tools_ASCII2Hex(p_Buffer, Length, &Buffer_Hex);

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CCSOSEND_BYTES(p_Socket->ID, Length, Buffer_Hex);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue);   
}

SIM70XX_Error_t SIM7020_TCP_TransmitString(const SIM7020_t* const p_Device, SIM7020_TCP_Socket_t* p_Socket, std::string Data)
{
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_Socket == NULL) || (Data.length() > 512) || (p_Socket->Type != SIM_TCP_TYPE_TCP))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->isConnected == false)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CCSOSEND_STRING(p_Socket->ID, Data);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue);   
}

SIM70XX_Error_t SIM7020_TCP_Destroy(const SIM7020_t* const p_Device, SIM7020_TCP_Socket_t* p_Socket)
{
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_Socket == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->isConnected == false)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CSOCL(p_Socket->ID);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue));

    p_Socket->isConnected = false;

    return SIM70XX_ERR_OK;
}

#endif