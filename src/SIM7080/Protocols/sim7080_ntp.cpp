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

#include "sim7080.h"
#include "sim7080_ntp.h"

#include "../../Core/Queue/sim70xx_queue.h"
#include "../../Core/Commands/sim70xx_commands.h"

#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7080_NTP";

SIM70XX_Error_t SIM7080_NTP_Sync(SIM7080_t& p_Device, SIM7080_PDP_Context_t* p_PDP, std::string Server, int8_t TimeZone, struct tm* const p_Time, SIM7080_NTP_Error_t* const p_Error)
{
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
    else if(p_PDP->Internal.isActive == false)
    {
        return SIM70XX_ERR_PDP_NOT_ACTIVE;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CNTP_W(Server, TimeZone, p_PDP->ID, SIM7080_NTP_MODE_BOTH);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CNTP;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    // NOTE: Status and Response are swapped here!
    SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Status, &Response);
    if(Status.find("OK") == std::string::npos)
    {
        return SIM70XX_ERR_FAIL;
    }

    SIM70XX_LOGI(TAG, "Response: %s", Response.c_str());

    Index = Response.find("+CNTP: ");
    Response.erase(Index, std::string("+CNTP: ").size());

    Index = Response.find(",");

    // No "," detected -> We have a response without time.
    if(Index == std::string::npos)
    {
        NTP_Error = static_cast<SIM7080_NTP_Error_t>(SIM70XX_Tools_StringToUnsigned(Response));
    }
    // "," detected -> We have a response with time.
    else
    {
        NTP_Error = static_cast<SIM7080_NTP_Error_t>(SIM70XX_Tools_StringToUnsigned(Response.substr(Index - 1, Index)));

        SIM70XX_LOGI(TAG, "NTP Error: %u", NTP_Error);

        if(NTP_Error == SIM7080_NTP_ERROR_OK)
        {
            Response.erase(0, Index + 1);

            SIM70XX_Tools_StringRemove(&Response);

            strptime((char*)Response.c_str(), "%Y/%m/%d,%H:%M:%S", p_Time);
        }
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

SIM70XX_Error_t SIM7080_NTP_GetTime(SIM7080_t& p_Device, struct tm* const p_Time, int8_t* const p_Timezone)
{
    size_t Index;
    std::string Status;
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Time == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CCLK_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    SIM70XX_Tools_StringRemove(&Response);

    // Get the timezone from the response.
    Index = Response.find("+");
    if(p_Timezone != NULL)
    {
        *p_Timezone = static_cast<int8_t>(SIM70XX_Tools_StringToSigned(Response.substr(Index + 1)));
    }
    Response.erase(Index);

    // We have to add the year to the response to use the convert function from time.h properly.
    Response = "20" + Response;
    SIM70XX_LOGI(TAG, "Response: %s", Response.c_str());

    strptime((char*)Response.c_str(), "%Y/%m/%d,%H:%M:%S", p_Time);

    return SIM70XX_ERR_OK;
}

#endif