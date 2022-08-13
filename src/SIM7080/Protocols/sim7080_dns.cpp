 /*
 * sim7080_dns.cpp
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

#include "sim7080.h"
#include "sim7080_tcpip.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7080_TCPIP";

// TODO: Add configuration for DNS Server (CDNSCFG)
// TODO: Add selection of PDP index for DNS (CDNSPDPID)

SIM70XX_Error_t SIM7080_TCP_ParseDNS(SIM7080_t& p_Device, std::string Host, std::string* p_IP, SIM7080_DNS_Error_t* p_Error, uint32_t Timeout)
{
    size_t Index;
    uint32_t Now;
    uint8_t DNS_Error;
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_IP == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CDNSGIP(Host);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    Now = SIM70XX_Tools_GetmsTimer();
    while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+CDNSGIP", &Response) == false)
    {
        if((SIM70XX_Tools_GetmsTimer() - Now) > (Timeout * 1000UL))
        {
            return SIM70XX_ERR_TIMEOUT;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    SIMXX_TOOLS_REMOVE_LINEEND(Response);

    // Filter out the error code.
    Index = Response.find(",");
    DNS_Error = std::stoi(Response.substr(Index - 1, 1));

    if(DNS_Error == 1)
    {
        // Remove the command and the error code
        Response.replace(0, Index + 1, "");

        // Filter out the domain.
        Response.erase(0, Response.find(",") + 1);

        // Filter out the IP address.
        Response.replace(Response.find("\""), std::string("\"").size(), "");
        *p_IP = Response.replace(Response.find("\""), std::string("\"").size(), "");

        return SIM70XX_ERR_OK;
    }
    // Handle the error codes.
    else if(DNS_Error != 1)
    {
        DNS_Error = std::stoi(Response.substr(Index + 1));

        ESP_LOGD(TAG, "DNS_Error: %u", DNS_Error);

        if (p_Error != NULL)
        {
            *p_Error = (SIM7080_DNS_Error_t)DNS_Error;
        }
    }

    return SIM70XX_ERR_FAIL;
}

#endif