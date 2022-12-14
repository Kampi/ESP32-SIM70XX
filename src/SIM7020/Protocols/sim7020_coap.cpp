 /*
 * sim7020_coap.cpp
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

#if((CONFIG_SIMXX_DEV == 7020) && (defined CONFIG_SIM70XX_DRIVER_WITH_COAP))

#include "sim7020.h"
#include "sim7020_coap.h"

#include "../../Core/Queue/sim70xx_queue.h"
#include "../../Core/Commands/sim70xx_commands.h"
#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7020_CoAP";

SIM70XX_Error_t SIM7020_CoAP_Create(SIM7020_t& p_Device, std::string Server, uint16_t Port, SIM7020_CoAP_Socket_t* p_Socket, uint8_t CID)
{
    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    p_Socket->Server = Server;
    p_Socket->Port = Port;
    p_Socket->Internal.ID = CID;
    p_Socket->Internal.isCreated = false;

    return SIM7020_CoAP_Create(p_Device, p_Socket);
}

SIM70XX_Error_t SIM7020_CoAP_Create(SIM7020_t& p_Device, SIM7020_CoAP_Socket_t* p_Socket)
{
    std::string Response;
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if((p_Socket == NULL) || (p_Socket->Internal.CID > 10))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    CommandStr = "AT+CCOAPNEW=\"" + p_Socket->Server + "\"," + std::to_string(p_Socket->Port) + "," + std::to_string(p_Socket->Internal.CID);
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CCOAPNEW(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    SIM70XX_LOGI(TAG, "Response: %s", Response.c_str());
    p_Socket->Internal.ID = static_cast<uint8_t>(SIM70XX_Tools_StringToUnsigned(Response));

    // Everything okay. The socket is active now.
    SIM70XX_LOGI(TAG, "Socket %u opened...", p_Socket->Internal.ID);

    p_Device.CoAP.Sockets.push_back(p_Socket);
    p_Socket->Internal.isCreated = true;

    return SIM70XX_ERR_OK;    
}

SIM70XX_Error_t SIM7020_CoAP_Transmit(SIM7020_t& p_Device, SIM7020_CoAP_Socket_t* p_Socket, const void* p_Buffer, uint16_t Length)
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

    SIM70XX_Tools_Buf2Hex(p_Buffer, Length, &Buffer_Hex);

    CommandStr = "AT+CCOAPSEND=" + std::to_string(p_Socket->Internal.ID) + "," + std::to_string(Length) + ",\"" + Buffer_Hex + "\"";
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CCOAPCSEND(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}
/*
SIM70XX_Error_t SIM7020_CoAP_Transmit(SIM7020_t& p_Device, SIM7020_CoAP_Socket_t* p_Socket, SIM7020_CoAP_Msg_t* p_Message)
{
    std::string CommandStr;
    std::string Buffer_Hex;
    std::string Token_Str;
    SIM70XX_TxCmd_t* Command;

    if((p_Socket == NULL) ||
       (p_Message == NULL) ||
       ((p_Message->Length > 0) && (p_Message->p_Payload == NULL)) ||
       (p_Message->Type > SIM7020_COAP_RST)
      )
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

    #if(BYTE_ORDER == LITTLE_ENDIAN)
        p_Message->Token = __builtin_bswap64(p_Message->Token);
    #endif

    // TBD

    SIM70XX_Tools_Buf2Hex(p_Message->Endpoint.c_str(), p_Message->Endpoint.length(), &Buffer_Hex);
    SIM70XX_Tools_Buf2Hex((const void*)&p_Message->Token, sizeof(p_Message->Token), &Token_Str);

    std::string Data = "AABB";

    CommandStr = "AT+CCOAPCSEND=" + std::to_string(p_Socket->Internal.ID) + ",1,0,0,2,\"0a\",\"b7" + Buffer_Hex + "\"," + std::to_string(Data.size()) + ",\"" + Data + "\"";
    //CommandStr = "AT+CCOAPCSEND=" + std::to_string(p_Socket->Internal.ID) + ",1," + std::to_string(p_Message->Type) + "," + std::to_string(p_Message->FunctionCode_H) + "," + std::to_string(p_Message->FunctionCode_L) + "," + Token_Str + ",\"ff" + Buffer_Hex + "\",,";
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CCOAPCSEND(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}
*/
SIM70XX_Error_t SIM7020_CoAP_Receive(SIM7020_t& p_Device, SIM7020_CoAP_Socket_t* p_Socket, std::string* p_Buffer)
{
    size_t Index;
    std::string Response;
    SIM70XX_TxCmd_t Command;

    if((p_Socket == NULL) || (p_Buffer == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->Internal.isDataReceived == false)
    {
        return SIM70XX_ERR_QUEUE_EMPTY;
    }
    else if(p_Socket->Internal.isCreated == false)
    {
        return SIM70XX_ERR_SOCKET_NOT_CREATED;
    }

    p_Buffer->clear();

    while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+CCOAPNMI: " + std::to_string(p_Socket->Internal.ID), &Response) == false);

    SIM70XX_LOGI(TAG, "Response: %s", Response.c_str());

    // Get the index of the first delimiter.
    Index = Response.find(",");

    // Get the index of the second delimiter.
    Index = Response.find(",", Index + 1);
    *p_Buffer = Response.substr(Index + 1);

    // Remove the last line end.
    if(p_Buffer->empty() == false)
    {
        p_Buffer->pop_back();
    }

    p_Socket->Internal.isDataReceived = false;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_CoAP_Destroy(SIM7020_t& p_Device, SIM7020_CoAP_Socket_t* p_Socket)
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
    *Command = SIM7020_AT_CCOAPDEL(p_Socket->Internal.ID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    // Remove the socket from the list with active sockets.
    for(std::vector<SIM7020_CoAP_Socket_t*>::iterator it = p_Device.CoAP.Sockets.begin(); it != p_Device.CoAP.Sockets.end(); ++it)
    {
        if((*it)->Internal.ID == p_Socket->Internal.ID)
        {
            p_Device.CoAP.Sockets.erase(it);

            break;
        }
    }

    p_Socket->Internal.isCreated = false;

    return SIM70XX_ERR_OK;
}

#endif