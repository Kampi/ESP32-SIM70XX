 /*
 * sim7080_ntp.cpp
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

#if((CONFIG_SIMXX_DEV == 7080) && (defined CONFIG_SIM70XX_DRIVER_WITH_NTP))

#include <esp_log.h>

#include "sim7080.h"
#include "sim7080_ntp.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7080_NTP";

SIM70XX_Error_t SIM7080_NTP_Sync(SIM7080_t& p_Device, std::string Server, int8_t TimeZone, struct tm* p_Time, SIM7080_NTP_Error_t* p_Error, uint8_t CID)
{
    struct tm;
    size_t Index;
    std::string Status;
    std::string Response;
    SIM7080_NTP_Error_t NTP_Error;
    SIM70XX_TxCmd_t* Command;
    SIM70XX_Error_t Error = SIM70XX_ERR_FAIL;

    if((TimeZone < -47) || (TimeZone > 48) || (p_Time == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CNTP_W(Server, TimeZone, CID, 2);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CNTP;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Status, &Response);

    if(Status.find("OK") == std::string::npos)
    {
        return SIM70XX_ERR_FAIL;
    }

    ESP_LOGI(TAG, "Response: %s", Response.c_str());

    Index = Response.find("+CNTP: ");
    Response.erase(Index, std::string("+CNTP: ").size());

    Index = Response.find(",");

    // No "," detected -> We have a response without time.
    if(Index == std::string::npos)
    {
        NTP_Error = (SIM7080_NTP_Error_t)std::stoi(Response);
    }
    // "," detected -> We have a response with time.
    else
    {
        NTP_Error = (SIM7080_NTP_Error_t)std::stoi(Response.substr(Index - 1, Index));
        Response.erase(0, Index + 1);

        Response.replace(Response.find("\""), std::string("\"").size(), "");
        Response.replace(Response.find("\""), std::string("\"").size(), "");

        strptime((char*)Response.c_str(), "%Y/%m/%d,%H:%M:%S", p_Time);
    }

    if(NTP_Error == SIM7080_NTP_ERROR_OK)
    {
        Error = SIM70XX_ERR_OK;
    }

    if(p_Error != NULL)
    {
        *p_Error = NTP_Error;
    }

    return Error;
}

#endif