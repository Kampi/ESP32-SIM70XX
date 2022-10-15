 /*
 * sim7080_gps.cpp
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

#if((CONFIG_SIMXX_DEV == 7080) && (defined CONFIG_SIM70XX_DRIVER_WITH_GPS))

#include <esp_log.h>

#include "sim7080.h"
#include "sim7080_gps.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7080_GPS";

SIM70XX_Error_t SIM7080_GPS_Config(SIM7080_t& p_Device, const SIM7080_GPS_Config_t const* p_Config)
{
    uint16_t Threshold;
    uint32_t Timeout;

    if((p_Config == NULL) || (p_Config->Port != SIM7080_GPS_PORT_OFF) ||
       (p_Config->Threshold > 0) && (p_Config->Threshold > 10000) ||
       (p_Config->Timeout > 0) && ((p_Config->Timeout < 10000) || (p_Config->Timeout > 180000))
       )
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    Threshold = p_Config->Threshold;
    if(p_Config->Threshold == 0)
    {
        Threshold = 10000;
    }

    Timeout = p_Config->Timeout;
    if(p_Config->Timeout == 0)
    {
        Timeout = 30000;
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_GPS_GetSingleLocation(SIM7080_t& p_Device, SIM7080_GPS_PwrLevel_t Power, SIM7080_GPS_Error_t* p_Error)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SGNSCMD(std::string("AT+SGNSCMD=" + std::to_string(SIM7080_GPS_MODE_ON_SINGLE) + "," + std::to_string(Power)));
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }

    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    while(true)
    {
        std::string Event;

        if(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "SGNSERR", &Event))
        {
            ESP_LOGE(TAG, "GPS error");
            break;
        }
        else if(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "SGNSCMD", &Event))
        {
            ESP_LOGI(TAG, "Receive data");
            break;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    return SIM70XX_ERR_OK;
}

#endif