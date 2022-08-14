 /*
 * sim7080_pdp_gprs.cpp
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
#include "PDP/sim7080_pdp_defs.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7080_PDP";

SIM70XX_Error_t SIM7080_PDP_GPRS_Define(SIM7080_t& p_Device, SIM7080_PDP_GPRS_Type_t Type, SIM70XX_APN_t APN, uint8_t PDP)
{
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;
    SIM70XX_Error_t Error;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if((PDP == 0) || (PDP > 15))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    if(Type == SIM7080_PDP_GPRS_IP)
    {
        CommandStr = "AT+CGDCONT=" + std::to_string(PDP) + ",\"IP\",\"" + APN.Name + "\"";
    }
    else if(Type == SIM7080_PDP_GPRS_IPV6)
    {
        CommandStr = "AT+CGDCONT=" + std::to_string(PDP) + ",\"IPV6\",\"" + APN.Name + "\"";
    }
    else if(Type == SIM7080_PDP_GPRS_IPV4V6)
    {
        CommandStr = "AT+CGDCONT=" + std::to_string(PDP) + ",\"IPV4V6\",\"" + APN.Name + "\"";
    }
    else if(Type == SIM7080_PDP_GPRS_NO_IP)
    {
        CommandStr = "AT+CGDCONT=" + std::to_string(PDP) + ",\"Non-IP\",\"" + APN.Name + "\"";
    }

    if((APN.Username.size() > 0) && (APN.Password.size() > 0))
    {
        CommandStr += ",\"" + APN.Username + "\",\"" + APN.Password + "\"";
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CGDCONT_W(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    // NOTE: We can not use the error macro here because Response and Status are swapped.
    Error = SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, NULL, &CommandStr);
    if(CommandStr.find("OK") == std::string::npos)
    {
        return Error;
    }

    return SIM70XX_ERR_OK;
}

bool SIM7080_PGP_GRPS_isAttached(SIM7080_t& p_Device)
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
    if((SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false) || (SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response) != SIM70XX_ERR_OK))
    {
        return false;
    }

    return (bool)std::stoi(Response);
}

#endif