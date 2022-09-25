 /*
 * sim7020_sntp.cpp
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

#if((CONFIG_SIMXX_DEV == 7020) && (defined CONFIG_SIM70XX_DRIVER_WITH_NTP))

#include <esp_log.h>

#include "sim7020.h"
#include "sim7020_ntp.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7020_NTP";

SIM70XX_Error_t SIM7020_NTP_Sync(SIM7020_t& p_Device, std::string Server, int8_t TimeZone, struct tm* p_Time, uint32_t Timeout)
{
    uint32_t Now;
    std::string Response;
    std::string Zone;
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if((TimeZone < -47) || (TimeZone > 48))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    // Stop an ongoing NTP request.
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CSNTPSTOP;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout);
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    // Start a new request.
    SIM70XX_CREATE_CMD(Command);

    if(TimeZone > 0)
    {
        *Command = SIM7020_AT_CSNTPSTART_TZ(Server, "+" + std::to_string(TimeZone));
    }
    else if(TimeZone < 0)
    {
        *Command = SIM7020_AT_CSNTPSTART_TZ(Server, "-" + std::to_string(TimeZone));
    }
    else
    {
        *Command = SIM7020_AT_CSNTPSTART(Server);
    }

    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    Now = SIM70XX_Tools_GetmsTimer();
    while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+CSNTP", &Response) == false)
    {
        if((SIM70XX_Tools_GetmsTimer() - Now) > (Timeout * 1000UL))
        {
            return SIM70XX_ERR_TIMEOUT;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    // Remove the command from the response.
    Response.replace(Response.find("+CSNTP:"), std::string("+CSNTP:").size(), "");

    ESP_LOGD(TAG, "Response: %s", Response.c_str());

    // Stop the NTP request.
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CSNTPSTOP;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    strptime((char*)Response.c_str(), "%Y/%m/%d,%H:%M:%S", p_Time);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_NTP_GetTime(SIM7020_t& p_Device, struct tm* p_Time, int8_t* p_Timezone)
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
    *Command = SIM7020_AT_CCLK_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    // Get the timezone from the response.
    Index = Response.find("+");
    if(p_Timezone != NULL)
    {
        *p_Timezone = std::stoi(Response.substr(Index + 1));
    }
    Response.erase(Index);

    // We have to add the year to the response to use the convert function from time.h properly.
    Response = "20" + Response;
    ESP_LOGD(TAG, "Response: %s", Response.c_str());

    strptime((char*)Response.c_str(), "%Y/%m/%d,%H:%M:%S", p_Time);

    return SIM70XX_ERR_OK;
}

#endif