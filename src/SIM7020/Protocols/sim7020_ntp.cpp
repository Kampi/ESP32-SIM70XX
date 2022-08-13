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

SIM70XX_Error_t SIM7020_NTP_GetNetworkTime(SIM7020_t& p_Device, std::string Server, int8_t Timezone, uint8_t Timeout)
{
    std::string Zone;
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if((Timezone < -47) || (Timezone > 48))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    if(Timezone > 0)
    {
        Zone = "+";
    }
    Zone += std::to_string(Timezone);

    // Stop the NTP request.
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CSNTPSTOP;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout);
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    // Start a new request.
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CSNTPSTART(Server, Zone);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    // TODO: Wait for the time response

    // Stop the NTP request.
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CSNTPSTOP;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout);

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

#endif