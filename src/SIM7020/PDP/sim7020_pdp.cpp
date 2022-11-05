 /*
 * sim7020_pdp.cpp
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

#if(CONFIG_SIMXX_DEV == 7020)

#include <esp_log.h>

#include "sim7020.h"
#include "sim7020_pdp_defs.h"
#include "../../Core/Queue/sim70xx_queue.h"
#include "../../Core/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7020_PDP";

// TODO: Check for GPRS and IP (see SIM7080)

SIM70XX_Error_t SIM7020_PDP_GetContext(SIM7020_t& p_Device, SIM7020_PDP_Context_t* const p_Context)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Context == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CGDCONT_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    // TODO: Get the informations from the input

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_PDP_Switch(SIM7020_t& p_Device, bool Enable, uint8_t Context)
{
    SIM70XX_TxCmd_t* Command;

    if(Context > 10)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CGACT_W(Context, Enable);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

SIM70XX_Error_t SIM7020_PDP_GetStatus(SIM7020_t& p_Device, std::vector<SIM7020_PDP_Status_t>* p_Status)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Status == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CGACT_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    do
    {
        uint32_t Index;
        std::string Dummy;
        SIM7020_PDP_Status_t Status;

        Dummy = Response.substr(0, 3);
        Response.erase(0, 3);

        Index = Dummy.find(",");
        if(Index != std::string::npos)
        {
            Status.CID = (uint8_t)SIM70XX_Tools_StringToUnsigned(Dummy.substr(Index - 1, 1));
            Status.Status = (bool)SIM70XX_Tools_StringToUnsigned(Dummy.substr(Index + 1, 1));
            Dummy.erase(0, Index + 1);
        }

        p_Status->push_back(Status);
    } while(Response.size() > 0);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_PDP_ReadDynamicParameters(SIM7020_t& p_Device, SIM7020_PDP_Params_t* p_Params)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Params == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    // Get the PDP type.
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_MCGDEFCONT_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    Response = SIM70XX_Tools_SubstringSplitErase(&Response);
    if(Response.find("IP") != std::string::npos)
    {
        p_Params->Type = SIM7020_PDP_IP;
    }
    else if(Response.find("IPV6") != std::string::npos)
    {
        p_Params->Type = SIM7020_PDP_IPV6;
    }
    else if(Response.find("IP4V6") != std::string::npos)
    {
        p_Params->Type = SIM7020_PDP_IPV4V6;
    }
    else if(Response.find("Non-IP") != std::string::npos)
    {
        p_Params->Type = SIM7020_PDP_NO_IP;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CGCONTRDP;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    p_Params->CID = (uint8_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response));
    p_Params->Baerer = (uint8_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response));
    p_Params->APN = SIM70XX_Tools_SubstringSplitErase(&Response);
    SIM70XX_Tools_StringRemove(&p_Params->APN);

    // Process the IP address and the subnet.
    SIM70XX_Tools_StringRemove(&Response);
    if(p_Params->Type == SIM7020_PDP_IP)
    {
        p_Params->IP = SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".");
        p_Params->Subnet = Response;
    }
    else if(p_Params->Type == SIM7020_PDP_IP)
    {
        p_Params->IP = SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".");
        p_Params->Subnet = Response;
    }

    ESP_LOGD(TAG, "Type: %u", p_Params->Type);
    ESP_LOGD(TAG, "IP: %s", p_Params->IP.c_str());
    ESP_LOGD(TAG, "Subnet: %s", p_Params->Subnet.c_str());
    ESP_LOGD(TAG, "Operator: %s", p_Params->APN.c_str());
    ESP_LOGD(TAG, "CID: %u", p_Params->CID);
    ESP_LOGD(TAG, "Baerer: %u", p_Params->Baerer);

    return SIM70XX_ERR_OK;
}

#endif