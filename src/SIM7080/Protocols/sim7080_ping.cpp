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

static const char* TAG = "SIM7080_TCPIP";

SIM70XX_Error_t SIM7080_TCP_Ping(SIM7080_t& p_Device, SIM7080_Ping_t& p_Config, std::vector<SIM7080_PingRes_t>* p_Result, bool IPv6)
{
    size_t Index;
    std::string Response;
    std::string CommandStr;
    SIM70XX_Error_t Error = SIM70XX_ERR_OK;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if((p_Result == NULL) || (p_Config.Retries == 0) || (p_Config.Size == 0) || (p_Config.Timeout == 0))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    p_Result->clear();

    if(p_Config.Retries <= -1)
    {
        p_Config.Retries = 4;
    }
    else
    {
        p_Config.Retries = std::min(p_Config.Retries, (int16_t)500);
    }

    if(p_Config.Size <= -1)
    {
        p_Config.Size = 32;
    }
    else
    {
        p_Config.Size = std::min(p_Config.Size, (int16_t)1400);
    }

    if(p_Config.Timeout <= -1)
    {
        p_Config.Timeout = 100;
    }
    else
    {
        p_Config.Timeout = std::min(p_Config.Timeout, (int32_t)60000);
    }

    if(IPv6)
    {
        CommandStr = "AT+SNPING6=";
    }
    else
    {
        CommandStr = "AT+SNPING4=";
    }

    CommandStr += "\"" + p_Config.IP + "\"," + std::to_string(p_Config.Retries) + "," + std::to_string(p_Config.Size) + "," + std::to_string(p_Config.Timeout);
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SNPING(CommandStr, (uint16_t)p_Config.Retries);
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
        Result.ReplyTime = std::stoi(SIM70XX_Tools_SubstringSplitErase(&Dummy));

        ESP_LOGD(TAG, "IP: %s", Result.IP.c_str());
        ESP_LOGD(TAG, "Reply time: %u", Result.ReplyTime);

        p_Result->push_back(Result);
    } while(Index != std::string::npos);

    return Error;
}

#endif