 /*
 * sim7020_psm.cpp
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

#if(CONFIG_SIMXX_DEV == 7020)

#include <bitset>

#include "sim7020.h"
#include "sim7020_pwrmgnt.h"

#include "../../Core/Queue/sim70xx_queue.h"
#include "../../Core/Commands/sim70xx_commands.h"

#include "../../Core/Arch/ESP32/Timer/sim70xx_timer.h"
#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7020_PSM";

SIM70XX_Error_t SIM7020_PSM_Init(SIM7020_t& p_Device, SIM7020_NetReg_t NetReg)
{
    SIM70XX_TxCmd_t* Command;

    if((NetReg != SIM7020_NETREG_PSM) && (NetReg != SIM7020_NETREG_PSM_EMM))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CEREG_W(SIM7020_NETREG_PSM);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CEREG_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

SIM70XX_Error_t SIM7020_PSM_Enable(SIM7020_t& p_Device, SIM7020_PSM_TAU_t TAU_Base, uint8_t TAU_Value, SIM7020_PSM_Time_t Time_Base, uint8_t Time_Value)
{
    uint32_t Now;
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if((TAU_Base > SIM7020_TAU_BASE_320_H) || (TAU_Value > 31) || (Time_Base > SIM7020_TIME_BASE_6_M) || (Time_Value > 31))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CPSMS(true, std::bitset<8>((TAU_Base << 5) | (TAU_Value & 0x1F)).to_string(), std::bitset<8>((Time_Base << 5) | (Time_Value & 0x1F)).to_string());
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    // Check if the "CEREG" response is send.
    Now = SIM70XX_Timer_GetMilliseconds();
    do
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);

        // No message received.
        if((SIM70XX_Timer_GetMilliseconds() - Now) > 5000)
        {
            SIM70XX_LOGI("TAG", "Update PSM settings...");

            break;
        }
    } while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+CEREG", &Response));

    p_Device.PwrMgnt.PSM.isEnabled = true;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_PSM_Disable(SIM7020_t& p_Device, SIM7020_PSM_Enable_t Mode)
{
    SIM70XX_TxCmd_t* Command;

    if(Mode == SIM7020_PSM_ENABLE)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CPSMS_DIS(Mode);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    p_Device.PwrMgnt.PSM.isEnabled = false;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_PSM_GetStatus(SIM7020_t& p_Device, bool* const p_Status)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Status == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CPSMS_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    *p_Status = (bool)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response));

    ESP_LOGI(TAG, "PSM Status: %u", *p_Status);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_PSM_GetEventStatus(SIM7020_t& p_Device, bool* const p_Enable)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Enable == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CPSMSTATUS_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    *p_Enable = (bool)SIM70XX_Tools_StringToUnsigned(Response);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_PSM_SetEventStatus(SIM7020_t& p_Device, bool Enable)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device.PwrMgnt.PSM.isEventEnabled == Enable)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CPSMSTATUS_W(Enable);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

#endif