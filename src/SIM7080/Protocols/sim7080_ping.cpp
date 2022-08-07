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

#include "sim7080.h"
#include "sim7080_tcpip.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7080_TCPIP";

SIM70XX_Error_t SIM7080_TCP_Ping4(SIM7080_t& p_Device)
{
    uint8_t Pings;
    std::string Status;
    std::string Response;
    std::string CommandStr;
    SIM70XX_Error_t Error = SIM70XX_ERR_OK;
    SIM70XX_TxCmd_t* Command;

    CommandStr = "AT+SNPING4=\"www.google.de\",3,16,1000";
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SNPING(CommandStr, 3);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    ESP_LOGI(TAG, "A: %s", Response.c_str());
    ESP_LOGI(TAG, "B: %u", Response.size());

    ESP_LOGD(TAG, "Ping successful");

    return Error;
}

#endif