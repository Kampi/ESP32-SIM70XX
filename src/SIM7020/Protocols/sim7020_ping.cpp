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

#include <esp_log.h>

#include <algorithm>

#include "sim7020.h"
#include "sim7020_tcpip.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7020_TCPIP";

SIM70XX_Error_t SIM7020_TCP_Ping(SIM7020_t& p_Device, SIM7020_Ping_t& p_Config, std::vector<SIM7020_PingRes_t>* p_Result)
{
    uint8_t Pings;
    std::string Status;
    std::string Response;
    std::string CommandStr;
    SIM70XX_Error_t Error = SIM70XX_ERR_OK;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    // TODO: Add Parameter check

    p_Result->clear();

    if(p_Config.Retries <= -1)
    {
        p_Config.Retries = 4;
    }
    else
    {
        p_Config.Retries = std::min(p_Config.Retries, (int8_t)100);
    }

    if(p_Config.DataLength <= -1)
    {
        p_Config.DataLength = 32;
    }
    else
    {
        p_Config.DataLength = std::min(p_Config.DataLength, (int16_t)5120);
    }

    if(p_Config.Timeout <= -1)
    {
        p_Config.Timeout = 100;
    }
    else
    {
        p_Config.Timeout = std::min(p_Config.Timeout, (int16_t)600);
    }

    // TODO: Copy implementation from SIM7080?

    CommandStr = "AT+CIPPING=\"" + p_Config.Host + "\"," + std::to_string(p_Config.Retries) + "," + std::to_string(p_Config.DataLength) + "," + std::to_string(p_Config.Timeout);
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CIPPING(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
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
            do
            {
                std::string Temp;
                SIM7020_PingRes_t Result;

                Temp = SIM70XX_Tools_SubstringSplitErase(&Response);
                SIMXX_TOOLS_REMOVE_LINEEND(Temp);

                Temp.replace(Temp.find("+CIPPING: "), std::string("+CIPPING: ").size(), "");

                // Filter out the reply ID.
                SIM70XX_Tools_SubstringSplitErase(&Temp);

                // Filter out the IP address.
                Result.IP =  SIM70XX_Tools_SubstringSplitErase(&Temp);

                // Filter out the reply time.
                Result.ReplyTime = std::stoi(SIM70XX_Tools_SubstringSplitErase(&Temp));

                // Filter out the time to live.
                Result.TTL = std::stoi(SIM70XX_Tools_SubstringSplitErase(&Temp));

                p_Result->push_back(Result);

                Pings++;

                ESP_LOGD(TAG, "Ping %u / %u", Pings, p_Config.Retries);
                ESP_LOGD(TAG, "     Response: %s", Response.c_str());
            } while(Response.find("+CIPPING") != std::string::npos);
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    } while(Pings != p_Config.Retries);

    return Error;
}

#endif