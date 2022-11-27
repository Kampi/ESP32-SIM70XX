 /*
 * sim7080_baerer.cpp
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

#include "sim7080.h"
#include "sim7080_baerer.h"

#include "../../Core/Queue/sim70xx_queue.h"
#include "../../Core/Commands/sim70xx_commands.h"

#include "../../Core/Arch/ESP32/Timer/sim70xx_timer.h"
#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

SIM70XX_Error_t SIM7080_Baerer_SetAPN(SIM7080_t& p_Device, SIM70XX_APN_t APN, SIM7080_PDP_Context_t* p_PDP, uint32_t Timeout)
{
    unsigned long Now;
    bool isAttached;

    if((p_PDP == NULL) || (p_PDP->ID > 3))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    if(p_Device.Connection.Functionality != SIM7080_FUNC_MIN)
    {
        SIM70XX_ERROR_CHECK(SIM7080_SetFunctionality(p_Device, SIM7080_FUNC_MIN));
    }

    SIM70XX_ERROR_CHECK(SIM7080_SetFunctionality(p_Device, SIM7080_FUNC_FULL));

    isAttached = false;
    Now = SIM70XX_Timer_GetMilliseconds();
    do
    {
        SIM70XX_Qual_t Report;

        SIM70XX_ERROR_CHECK(SIM7080_Info_GetQuality(p_Device, &Report));
        if(Report.RSSI != 99)
        {
            isAttached = SIM7080_Baerer_GRPS_isAttached(p_Device);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } while((isAttached == false) && ((SIM70XX_Timer_GetMilliseconds() - Now) < (Timeout * 1000)));

    if(isAttached == false)
    {
        return SIM70XX_ERR_TIMEOUT;
    }

    SIM70XX_ERROR_CHECK(SIM7080_Baerer_PDP_Configure(p_Device, APN, p_PDP));

    return SIM7080_Baerer_PDP_Enable(p_Device, p_PDP);
}

#endif