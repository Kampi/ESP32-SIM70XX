 /*
 * sim7020_pdp_gprs.cpp
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

#include "sim7020.h"
#include "sim7020_baerer.h"

#include "../../Core/Queue/sim70xx_queue.h"
#include "../../Core/Commands/sim70xx_commands.h"

#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7080_Baerer";

SIM70XX_Error_t SIM7020_Baerer_GRPS_Attach(SIM7020_t& p_Device)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT_CGATT_W(true);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, NULL);
}

SIM70XX_Error_t SIM7020_Baerer_GRPS_Deattach(SIM7020_t& p_Device)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT_CGATT_W(false);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, NULL);
}

bool SIM7020_Baerer_GRPS_isAttached(SIM7020_t& p_Device, bool* const p_Deactivated)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return false;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT_CGATT_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if((SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false) || (SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response) != SIM70XX_ERR_OK))
    {
        return false;
    }

    if(Response.find("NO CARRIER") != std::string::npos)
    {
        *p_Deactivated = true;

        return false;
    }

    *p_Deactivated = false;

    return (bool)SIM70XX_Tools_StringToUnsigned(Response);
}

SIM70XX_Error_t SIM7020_Baerer_PDP_Configure(SIM7020_t& p_Device, SIM70XX_APN_t APN, SIM7020_PDP_Context_t* const p_PDP)
{
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;
    SIM70XX_Error_t Error;

    if((p_PDP == NULL) || (p_PDP->ID == 0) || (p_PDP->ID > 15))
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

    p_PDP->Internal.isConfigured = false;

    if(p_PDP->Type == SIM7020_PDP_IP)
    {
        CommandStr = std::to_string(p_PDP->ID) + ",\"IP\",\"" + APN.Name + "\"";
    }
    else if(p_PDP->Type == SIM7020_PDP_IPV6)
    {
        CommandStr = std::to_string(p_PDP->ID) + ",\"IPV6\",\"" + APN.Name + "\"";
    }
    else if(p_PDP->Type == SIM7020_PDP_IPV4V6)
    {
        CommandStr = std::to_string(p_PDP->ID) + ",\"IPV4V6\",\"" + APN.Name + "\"";
    }
    else if(p_PDP->Type == SIM7020_PDP_IP)
    {
        CommandStr = std::to_string(p_PDP->ID) + ",\"Non-IP\",\"" + APN.Name + "\"";
    }

    if((APN.Username.size() > 0) && (APN.Password.size() > 0))
    {
        CommandStr += ",\"" + APN.Username + "\",\"" + APN.Password + "\"";
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CGDCONT_W(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    // NOTE: We can not use the error macro here because Response and Status are swapped.
    Error = SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, NULL, &CommandStr);
    if(CommandStr.find("OK") == std::string::npos)
    {
        return Error;
    }

    p_PDP->Internal.isConfigured = true;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_Baerer_PDP_Enable(SIM7020_t& p_Device, SIM7020_PDP_Context_t* const p_PDP)
{
    std::string Status;
    SIM70XX_TxCmd_t* Command;

    if((p_PDP == NULL) || (p_PDP->ID == 0) || (p_PDP->ID > 15))
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
    *Command = SIM7020_AT_CGACT_W(p_PDP->ID, true);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, NULL, &Status));

    if(Status.find("OK") != std::string::npos)
    {
        SIM70XX_LOGI(TAG, "Activate PDP context: %u", p_PDP->ID);

        p_PDP->Internal.isActive = true;

        return SIM70XX_ERR_OK;
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7020_Baerer_PDP_Disable(SIM7020_t& p_Device, SIM7020_PDP_Context_t* const p_PDP)
{
    std::string Status;
    SIM70XX_TxCmd_t* Command;

    if((p_PDP == NULL) || (p_PDP->ID == 0) || (p_PDP->ID > 15))
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
    *Command = SIM7020_AT_CGACT_W(p_PDP->ID, false);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, NULL, &Status));

    if(Status.find("OK") != std::string::npos)
    {
        SIM70XX_LOGI(TAG, "Activate PDP context: %u", p_PDP->ID);

        p_PDP->Internal.isActive = false;

        return SIM70XX_ERR_OK;
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7020_Baerer_PDP_DisableAll(SIM7020_t& p_Device)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    for(uint8_t i = 1; i < 16; i++)
    {
        SIM70XX_CREATE_CMD(Command);
        *Command = SIM7020_AT_CGACT_W(false, i);
        SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
        if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
        {
            return SIM70XX_ERR_FAIL;
        }

        // NOTE: We ignore the error check at this point, because an error is reported when the PDP context is already disabled.
        SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_PDP_GetStatus(SIM7020_t& p_Device, SIM7020_PDP_Context_t* const p_PDP)
{
    uint8_t ID;
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if((p_PDP == NULL) || (p_PDP->ID == 0) || (p_PDP->ID > 15))
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
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    ID = (uint8_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response));
    if(ID == p_PDP->ID)
    {
        p_PDP->Internal.isActive = (bool)SIM70XX_Tools_StringToUnsigned(Response);
    }

    return SIM70XX_ERR_OK;
}

#endif