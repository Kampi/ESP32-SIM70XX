 /*
 * sim7080_info.cpp
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

#if(CONFIG_SIMXX_DEV == 7080)

#include <esp_log.h>

#include "sim7080.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim7080_commands.h"

static const char* TAG = "SIM7080_Info";

SIM70XX_Error_t SIM7080_Info_GetNetworkRegistrationStatus(SIM7080_t& p_Device)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CGREG;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    p_Device.Connection.Status = (SIM7080_NetRegistration_t)std::stoi(Response.substr(Response.find(",") + 1));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_Info_GetQuality(SIM7080_t& p_Device)
{
    int8_t RSSI;
    uint8_t RXQual;
    size_t Index;
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CSQ;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    Index = Response.find(",");
    if(Index == std::string::npos)
    {
        return SIM70XX_ERR_FAIL;
    }

    RSSI = std::stoi(Response.substr(0, Index));
    RXQual = std::stoi(Response.substr(Response.find_last_of(",") + 1));

    // TODO: Calc correct values
    if(RSSI == 0)
    {
        p_Device.Connection.RSSI = -110;
    }
    else if(RSSI == 1)
    {
        p_Device.Connection.RSSI = -108;
    }
    else if(RSSI == 2)
    {
        p_Device.Connection.RSSI = -106;
    }
    // Simple approach to convert the return value from the modem into a RSSI value.
    else if((RSSI >= 3) || (RSSI <= 30))
    {
        p_Device.Connection.RSSI = -105 + (2 * (RSSI - 3));
    }
    else
    {
        p_Device.Connection.RSSI = 0;
    }

    p_Device.Connection.RXQual = RXQual;

    if((RSSI == 0) && (RXQual == 0))
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_ERR_OK;
}

#endif