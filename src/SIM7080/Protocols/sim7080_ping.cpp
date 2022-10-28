 /*
 * sim7080_ping.cpp
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

#if((CONFIG_SIMXX_DEV == 7080) && (defined CONFIG_SIM70XX_DRIVER_WITH_TCPIP))

#include <esp_log.h>

#include <algorithm>

#include "sim7080.h"
#include "sim7080_tcpip.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

SIM70XX_Error_t SIM7080_TCPIP_Ping(SIM7080_t& p_Device, const SIM7080_Ping_t* const p_Config, std::vector<SIM7080_PingRes_t>* p_Result, bool IPv6)
{
    size_t Index;
    uint8_t Retries;
    uint16_t Size;
    uint16_t Timeout;
    std::string Response;
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if((p_Result == NULL) || (p_Config == NULL) || (p_Config->Retries == 0) || (p_Config->Size == 0) || (p_Config->Timeout == 0))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    p_Result->clear();

    if(p_Config->Retries <= -1)
    {
        Retries = 4;
    }
    else
    {
        Retries = std::min(p_Config->Retries, (int16_t)500);
    }

    if(p_Config->Size <= -1)
    {
        Size = 32;
    }
    else
    {
        Size = std::min(p_Config->Size, (int16_t)1400);
    }

    if(p_Config->Timeout <= -1)
    {
        Timeout = 100;
    }
    else
    {
        Timeout = std::min(p_Config->Timeout, (int32_t)60000);
    }

    SIM70XX_CREATE_CMD(Command);

    if(IPv6)
    {
        *Command = SIM7080_AT_SNPING6(p_Config->Host, Retries, Size, Timeout, (uint16_t)Retries);
    }
    else
    {
        *Command = SIM7080_AT_SNPING4(p_Config->Host, Retries, Size, Timeout, (uint16_t)Retries);
    }

    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    do
    {
        std::string Dummy;
        SIM7080_PingRes_t Result;

        Index = Response.find("\n");
        Dummy = Response.substr(0, Index).c_str();
        Response.erase(0, Index + 1);

        SIM70XX_Tools_SubstringSplitErase(&Dummy);
        Result.IP = SIM70XX_Tools_SubstringSplitErase(&Dummy);
        Result.ReplyTime = (uint16_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Dummy));

        p_Result->push_back(Result);
    } while(Index != std::string::npos);

    return SIM70XX_ERR_OK;
}

#endif