 /*
 * sim7080_pwrmgnt.cpp
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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include "sim7080.h"
#include "sim7080_pwrmgnt.h"
#include "../../Private/GPIO/sim70xx_gpio.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

SIM70XX_Error_t SIM7080_PwrMgnt_WakeUp(SIM7080_t& p_Device, uint8_t Timeout)
{
    uint32_t Now;
    SIM70XX_Error_t Error;

    if((p_Device.PwrMgnt.PSM.isEnabled == false) || (p_Device.PwrMgnt.PSM.isActive == false))
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_GPIO_SetPwrKey(true);

    Now = SIM70XX_Timer_GetMilliseconds();
    Error = SIM70XX_ERR_FAIL;
    do
    {
        Error = SIM7080_Ping(p_Device);

        if((SIM70XX_Timer_GetMilliseconds() - Now) > (Timeout * 1000UL))
        {
            Error = SIM70XX_ERR_TIMEOUT;
            goto SIM7080_PwrMgnt_WakeUpPSM_Error;

            break;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    } while(Error != SIM70XX_ERR_OK);

    p_Device.PwrMgnt.PSM.isActive = false;

SIM7080_PwrMgnt_WakeUpPSM_Error:
    SIM70XX_GPIO_SetPwrKey(false);

    return Error;
}

#endif