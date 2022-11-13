 /*
 * sim7080_coap.cpp
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

#if((CONFIG_SIMXX_DEV == 7080) && (defined CONFIG_SIM70XX_DRIVER_WITH_COAP))

#include <esp_log.h>

#include "sim7080.h"
#include "sim7080_coap.h"
#include "../../Core/Queue/sim70xx_queue.h"
#include "../../Core/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7080_CoAP";

SIM70XX_Error_t SIM7080_CoAP_Create(SIM7080_t& p_Device, std::string Server, uint16_t Port, SIM7080_CoAP_Socket_t* p_Socket)
{
    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    p_Socket->Server = Server;
    p_Socket->Port = Port;
    p_Socket->Internal.isCreated = false;

    return SIM7080_CoAP_Create(p_Device, p_Socket);
}

SIM70XX_Error_t SIM7080_CoAP_Create(SIM7080_t& p_Device, SIM7080_CoAP_Socket_t* p_Socket)
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
    else if(p_Socket->Internal.isCreated == true)
    {
        return SIM70XX_ERR_NOT_CONNECTED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CCOAPINIT;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CCOAPURL(p_Socket->Server, p_Socket->Port);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    // Everything okay. The socket is active now.
    ESP_LOGI(TAG, "Socket opened...");

    p_Device.CoAP.Socket = p_Socket;
    p_Socket->Internal.isCreated = true;

    return SIM70XX_ERR_OK;    
}

SIM70XX_Error_t SIM7080_CoAP_Transmit(SIM7080_t& p_Device, SIM7080_CoAP_Socket_t* p_Socket, const void* p_Buffer, uint16_t Length)
{
    std::string Buffer_Hex;
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if((p_Socket == NULL) || (p_Buffer == NULL) || (Length < 4) || (Length > 512))
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

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);   
}

SIM70XX_Error_t SIM7080_CoAP_Destroy(SIM7080_t& p_Device, SIM7080_CoAP_Socket_t* p_Socket)
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
    *Command = SIM7080_AT_CCOAPTERM;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    p_Socket->Internal.isCreated = false;

    return SIM70XX_ERR_OK;
}

#endif