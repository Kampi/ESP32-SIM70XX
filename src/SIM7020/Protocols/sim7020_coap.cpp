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

#include <esp_log.h>

#include "sim7020.h"
#include "sim7020_coap.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7020_CoAP";

SIM70XX_Error_t SIM7020_CoAP_Create(SIM7020_t& p_Device, std::string Server, uint16_t Port, SIM7020_CoAP_Socket_t* p_Socket, uint8_t CID)
{
    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    p_Socket->Server = Server;
    p_Socket->Port = Port;
    p_Socket->ID = CID;

    return SIM7020_CoAP_Create(p_Device, p_Socket);
}

SIM70XX_Error_t SIM7020_CoAP_Create(SIM7020_t& p_Device, SIM7020_CoAP_Socket_t* p_Socket)
{
    std::string Response;
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if((p_Socket == NULL) || (p_Socket->CID > 10))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->isConnected == false)
    {
        return SIM70XX_ERR_NOT_CONNECTED;
    }

    CommandStr = "AT+CCOAPNEW=\"" + p_Socket->Server + "\"," + std::to_string(p_Socket->Port) + "," + std::to_string(p_Socket->CID);
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CCOAPNEW(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    p_Socket->ID = (uint8_t)std::stoi(Response);

    // Everything okay. The socket is active now.
    ESP_LOGI(TAG, "Socket %u opened...", p_Socket->ID);

    p_Device.CoAP.Sockets.push_back(p_Socket);
    p_Socket->isConnected = true;

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
    else if(p_Socket->isConnected == false)
    {
        return SIM70XX_ERR_NOT_CONNECTED;
    }

    SIM70XX_Tools_ASCII2Hex(p_Buffer, Length, &Buffer_Hex);

    CommandStr = "AT+CCOAPCSEND=" + std::to_string(p_Socket->ID) + "," + std::to_string(Length) + ",\"" + Buffer_Hex + "\"";
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CCOAPCSEND(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);   
}

SIM70XX_Error_t SIM7020_CoAP_Transmit(SIM7020_t& p_Device, SIM7020_CoAP_Socket_t* p_Socket, SIM7020_CoAP_Type_t Type, uint8_t FunctionCode)
{
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;
    uint8_t FunctionCode_H;
    uint8_t FunctionCode_L;

    FunctionCode_H = FunctionCode >> 5;
    FunctionCode_L = FunctionCode & 0x1F;

    if((p_Socket == NULL) || (Type > SIM7020_COAP_RST) || (FunctionCode_H == 1) || (FunctionCode_H > 5))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    CommandStr = "AT+CCOAPCSEND=" + std::to_string(p_Socket->ID) + ",1," + std::to_string(Type) + "," + std::to_string(FunctionCode_H) + "," + std::to_string(FunctionCode_L);
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CCOAPCSEND(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);    
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
    else if(p_Socket->isConnected == false)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CCOAPDEL(p_Socket->ID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    p_Socket->isConnected = false;

    return SIM70XX_ERR_OK;
}

#endif