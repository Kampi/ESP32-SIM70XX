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
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

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
            Status.CID = std::stoi(Dummy.substr(Index - 1, 1));
            Status.Status = (bool)std::stoi(Dummy.substr(Index + 1, 1));
            Dummy.erase(0, Index + 1);
        }

        p_Status->push_back(Status);
    } while(Response.length() > 0);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_PDP_ReadDynamicParameters(SIM7020_t& p_Device)
{
    uint8_t Parts = 0;
    std::string Dummy;
    std::string Octett;
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CGCONTRDP;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    // TODO: Wait for the Response event.

/* TODO
    if(p_Device.PDP_Type == SIM7020_PDP_IPV6)
    {
        Parts = 16;
    }
    else
    {
        Parts = 4;
    }
*/
    // Get the IP address and the subnet mask.
    Dummy = Response.substr(Response.find_last_of(",") + 1);
    Response.erase(Response.find("," + Dummy), std::string("," + Dummy).length());
    Dummy.erase(std::remove(Dummy.begin(), Dummy.end(), '\"'), Dummy.end()); 

    // Filter out the IP address.
    for(uint8_t i = 0; i < (Parts - 1); i++)
    {
        Octett = SIM70XX_Tools_SubstringSplitErase(&Dummy);
        p_Device.PDP.IP += Octett + ".";
    }
    Octett = SIM70XX_Tools_SubstringSplitErase(&Dummy);
    p_Device.PDP.IP += Octett;

    // Filter the subnet mask.
    for(uint8_t i = 0; i < (Parts - 1); i++)
    {
        Octett = SIM70XX_Tools_SubstringSplitErase(&Dummy);
        p_Device.PDP.Subnet += Octett + ".";
    }
    Octett = SIM70XX_Tools_SubstringSplitErase(&Dummy);
    p_Device.PDP.Subnet += Octett;

    // Get the operator.
    Dummy = Response.substr(Response.find_last_of(",") + 1);
    Response.replace(Response.find("," + Dummy), std::string("," + Dummy).size(), "");
    Dummy.replace(Dummy.find("\""), std::string("\"").size(), "");
    p_Device.PDP.APN += Dummy;

    // Get the CID.
    Dummy = Response.substr(Response.find_last_of(",") + 1);
    Response.replace(Response.find("," + Dummy), std::string("," + Dummy).size(), "");
    p_Device.PDP.CID = std::stoi(Dummy);

    // Get the Baerer.
    p_Device.PDP.Baerer = std::stoi(Response);

    ESP_LOGD(TAG, "IP: %s", p_Device.PDP.IP.c_str());
    ESP_LOGD(TAG, "Subnet: %s", p_Device.PDP.Subnet.c_str());
    ESP_LOGD(TAG, "Operator: %s", p_Device.PDP.APN.c_str());
    ESP_LOGD(TAG, "CID: %u", p_Device.PDP.CID);
    ESP_LOGD(TAG, "Baerer: %u", p_Device.PDP.Baerer);

    return SIM70XX_ERR_OK;
}

#endif