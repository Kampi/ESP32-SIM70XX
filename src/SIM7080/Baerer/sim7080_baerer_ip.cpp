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

#include "sim7080.h"
#include "sim7080_baerer.h"

#include "../../Core/Queue/sim70xx_queue.h"
#include "../../Core/Commands/sim70xx_commands.h"

#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7080_Baerer_IP";

SIM70XX_Error_t SIM7080_Baerer_PDP_Configure(SIM7080_t& p_Device, SIM70XX_APN_t APN, SIM7080_PDP_Context_t* const p_PDP)
{
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if((p_PDP == NULL) || (p_PDP->ID > 3) || ((p_PDP->Auth != SIM7080_PDP_IP_AUTH_NONE) && (APN.Username.size() == 0)))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_PDP->Internal.isConfigured == true)
    {
        return SIM70XX_ERR_OK;
    }

    CommandStr = std::to_string(p_PDP->ID) + "," + std::to_string(p_PDP->Type) + ",\"" + APN.Name + "\"";

    if(p_PDP->Auth != SIM7080_PDP_IP_AUTH_NONE)
    {
        CommandStr += ",\"" + APN.Username + "\",\"" + APN.Password + "\"," + std::to_string(p_PDP->Auth);
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CNCFG_W(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    p_PDP->Internal.isConfigured = true;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_Baerer_PDP_Enable(SIM7080_t& p_Device, SIM7080_PDP_Context_t* const p_PDP)
{
    size_t Index;
    std::string Status;
    SIM70XX_TxCmd_t* Command;

    if((p_PDP == NULL) || (p_PDP->ID > 3))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_PDP->Internal.isConfigured == false)
    {
        return SIM70XX_ERR_PDP_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CNACT_W(p_PDP->ID, SIM7080_PDP_ENABLE);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, NULL, &Status);

    Index = Status.find("+APP PDP: ");
    if(Index == std::string::npos)
    {
        return SIM70XX_ERR_FAIL;
    }

    Status.erase(Index, std::string("+APP PDP: ").size());

    p_PDP->ID = (uint8_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Status));

    if(Status.find("ACTIVE") != std::string::npos)
    {
        p_PDP->Internal.isActive = true;

        SIM70XX_LOGI(TAG, "Activate PDP context: %u", p_PDP->ID);

        return SIM70XX_ERR_OK;
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7080_Baerer_PDP_Disable(SIM7080_t& p_Device, SIM7080_PDP_Context_t* const p_PDP)
{
    size_t Index;
    std::string Status;
    SIM70XX_TxCmd_t* Command;

    if((p_PDP == NULL) || (p_PDP->ID > 3))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_PDP->Internal.isConfigured == false)
    {
        return SIM70XX_ERR_PDP_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CNACT_W(p_PDP->ID, SIM7080_PDP_DISABLE);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, NULL, &Status);

    Index = Status.find("+APP PDP: ");
    if(Index == std::string::npos)
    {
        return SIM70XX_ERR_FAIL;
    }

    Status.erase(Index, std::string("+APP PDP: ").size());

    p_PDP->ID = (uint8_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Status));

    if(Status.find("DEACTIVE") != std::string::npos)
    {
        p_PDP->Internal.isActive = false;

        SIM70XX_LOGI(TAG, "Deactivate PDP context: %u", p_PDP->ID);

        return SIM70XX_ERR_OK;
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7080_Baerer_PDP_DisableAll(SIM7080_t& p_Device)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    for(uint8_t i = 0; i < 4; i++)
    {
        SIM70XX_CREATE_CMD(Command);
        *Command = SIM7080_AT_CNACT_W(i, SIM7080_PDP_DISABLE);
        SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
        if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
        {
            return SIM70XX_ERR_FAIL;
        }
        SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_Baerer_IP_CheckNetworks(SIM7080_t& p_Device, std::vector<SIM7080_PDP_Network_t>* const p_Networks)
{
    size_t Index;
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Networks == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CNACT_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

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

        Result.ID = (uint8_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Dummy));
        Result.Status = (SIM7080_PDP_Status_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Dummy));
        Result.IP = SIM70XX_Tools_SubstringSplitErase(&Dummy);

        p_Networks->push_back(Result);
    } while(Index != std::string::npos);

    return SIM70XX_ERR_OK;
}

#endif