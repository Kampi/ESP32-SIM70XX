 /*
 * sim7080_pdp.cpp
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

#if(CONFIG_SIMXX_DEV == 7080)

#include <esp_log.h>

#include "sim7080.h"
#include "sim7080_pdp_defs.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7080_PDP";

SIM70XX_Error_t SIM7080_PDP_IP_Configure(SIM7080_t& p_Device, SIM7080_PDP_IP_Type_t Type, SIM70XX_APN_t APN, uint8_t PDP, SIM7080_PDP_IP_Auth_t Auth)
{
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if((PDP > 3) || ((Auth != SIM7080_PDP_IP_AUTH_NONE) && (APN.Username.size() == 0)))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    CommandStr = std::to_string(PDP) + "," + std::to_string(Type) + ",\"" + APN.Name + "\"";

    if(Auth != SIM7080_PDP_IP_AUTH_NONE)
    {
        CommandStr += ",\"" + APN.Username + "\",\"" + APN.Password + "\"," + std::to_string(Auth);
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CNCFG_W(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);;
}

SIM70XX_Error_t SIM7080_PDP_IP_Action(SIM7080_t& p_Device, uint8_t PDP, SIM7080_PDP_Action_t Action)
{
    SIM70XX_TxCmd_t* Command;

    if(PDP > 3)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CNACT_W(PDP, Action);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

SIM70XX_Error_t SIM7080_PDP_IP_CheckNetworks(SIM7080_t& p_Device, std::vector<SIM7080_PDP_Network_t>* p_Networks)
{
    size_t Index;
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Networks == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CNACT_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    ESP_LOGI(TAG, "Response: %s", Response.c_str());

    if(Response.find("NOT READY") != std::string::npos)
    {
        return SIM70XX_ERR_FAIL;
    }

    do
    {
        std::string Dummy;
        SIM7080_PDP_Network_t Result;

        Index = Response.find("\n");
        Dummy = Response.substr(0, Index).c_str();
        Response.erase(0, Index + 1);

        Result.ID = std::stoi(SIM70XX_Tools_SubstringSplitErase(&Dummy));
        Result.Status = (SIM7080_PDP_Status_t)std::stoi(SIM70XX_Tools_SubstringSplitErase(&Dummy));
        Result.IP = SIM70XX_Tools_SubstringSplitErase(&Dummy);

        p_Networks->push_back(Result);
    } while(Index != std::string::npos);

    return SIM70XX_ERR_OK;
}

#endif