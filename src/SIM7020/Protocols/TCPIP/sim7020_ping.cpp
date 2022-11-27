 /*
 * sim7020_ping.cpp
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

#if((CONFIG_SIMXX_DEV == 7020) && (defined CONFIG_SIM70XX_DRIVER_WITH_TCPIP))

#include <algorithm>

#include "sim7020.h"
#include "sim7020_tcpip.h"
#include "../../../Core/Queue/sim70xx_queue.h"
#include "../../../Core/Commands/sim70xx_commands.h"

#include "../../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7020_TCPIP";

SIM70XX_Error_t SIM7020_TCPIP_Ping(SIM7020_t& p_Device, const SIM7020_Ping_t* const p_Config, std::vector<SIM7020_PingRes_t>* const p_Result)
{
    uint8_t Retries;
    uint8_t Pings;
    uint16_t DataLength;
    uint16_t Timeout;
    std::string Status;
    std::string Response;
    std::string CommandStr;
    SIM70XX_Error_t Error = SIM70XX_ERR_OK;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if((p_Result == NULL) || (p_Config == NULL) || (p_Config->Retries == 0) || (p_Config->Timeout == 0))
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
        Retries = std::min(p_Config->Retries, (int8_t)100);
    }

    if(p_Config->DataLength <= -1)
    {
        DataLength = 32;
    }
    else
    {
        DataLength = std::min(p_Config->DataLength, (int16_t)5120);
    }

    if(p_Config->Timeout <= -1)
    {
        Timeout = 100;
    }
    else
    {
        Timeout = std::min(p_Config->Timeout, (int16_t)600);
    }

    CommandStr = "AT+CIPPING=\"" + p_Config->Host + "\"," + std::to_string(Retries) + "," + std::to_string(DataLength) + "," + std::to_string(Timeout);
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CIPPING(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    Pings = 0;
    do
    {
        // Wait for a ping response.
        if(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+CIPPING", &Response))
        {
            // Split the received lines.
            do
            {
                SIM7020_PingRes_t Result;

                Response.replace(Response.find("+CIPPING: "), std::string("+CIPPING: ").size(), "");

                // Filter out the reply ID.
                SIM70XX_Tools_SubstringSplitErase(&Response);

                // Filter out the IP address.
                Result.IP =  SIM70XX_Tools_SubstringSplitErase(&Response);

                // Filter out the reply time.
                Result.ReplyTime = (uint8_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response));

                // Filter out the time to live.
                Result.TTL = (uint8_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response, "\n"));

                p_Result->push_back(Result);

                Pings++;

                SIM70XX_LOGD(TAG, "Ping %u / %u", Pings, p_Config->Retries);
            } while(Response.find("+CIPPING") != std::string::npos);
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    } while(Pings != p_Config->Retries);

    return Error;
}

#endif