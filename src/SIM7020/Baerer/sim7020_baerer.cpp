 /*
 * sim7020_baerer.cpp
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

#include "sim7020.h"
#include "sim7020_baerer_defs.h"

#include "../../Core/Queue/sim70xx_queue.h"
#include "../../Core/Commands/sim70xx_commands.h"

#include "../../Core/Arch/ESP32/Timer/sim70xx_timer.h"
#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7020_Baerer";

SIM70XX_Error_t SIM7020_Baerer_SetAPN(SIM7020_t& p_Device, SIM70XX_APN_t APN, SIM7020_PDP_Context_t* const p_PDP, uint32_t Timeout)
{
    unsigned long Now;
    bool isAttached;

    if((p_PDP == NULL) || (p_PDP->ID == 0) || (p_PDP->ID > 15))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_ERROR_CHECK(SIM7020_SetFunctionality(p_Device, SIM7020_FUNC_MIN));
    SIM70XX_ERROR_CHECK(SIM7020_Baerer_PDP_Configure(p_Device, APN, p_PDP));
    SIM70XX_ERROR_CHECK(SIM7020_SetFunctionality(p_Device, SIM7020_FUNC_FULL));
/*
    if(p_PDP->Internal.isActive == false)
    {
        SIM70XX_ERROR_CHECK(SIM7020_Baerer_PDP_Enable(p_Device, p_PDP));
    }
*/
    isAttached = false;
    Now = SIM70XX_Timer_GetMilliseconds();
    do
    {
        bool Deactivated;
        SIM70XX_Qual_t Report;

        SIM70XX_ERROR_CHECK(SIM7020_Info_GetQuality(p_Device, &Report));
        if(Report.RSSI != 99)
        {
            isAttached = SIM7020_Baerer_GRPS_isAttached(p_Device, &Deactivated);
            if(Deactivated == true)
            {
                SIM70XX_LOGE(TAG, "Context not activated!");

                return SIM70XX_ERR_PDP_NOT_ACTIVE;
            }
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } while((isAttached == false) && ((SIM70XX_Timer_GetMilliseconds() - Now) < (Timeout * 1000)));

    if(isAttached == false)
    {
        return SIM70XX_ERR_TIMEOUT;
    }

    SIM70XX_ERROR_CHECK(SIM7020_PDP_GetStatus(p_Device, p_PDP));

    if(p_PDP->Internal.isActive == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_PDP_ReadDynamicParameters(SIM7020_t& p_Device, SIM7020_PDP_Context_t* const p_Context, SIM7020_PDP_Params_t* const p_Params)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if((p_Context == NULL) || (p_Params == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Context->Internal.isActive == false)
    {
        return SIM70XX_ERR_PDP_NOT_ACTIVE;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CGCONTRDP_W(p_Context->ID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    p_Params->CID = static_cast<uint8_t>(SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response)));
    p_Params->Baerer = static_cast<uint8_t>(SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response)));
    p_Params->APN = SIM70XX_Tools_SubstringSplitErase(&Response);
    SIM70XX_Tools_StringRemove(&p_Params->APN);

    SIM70XX_Tools_StringRemove(&Response);
    if(p_Params->Type == SIM7020_PDP_IP)
    {
        p_Params->IP = SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".");
        p_Params->Subnet = Response;
    }
    else if(p_Params->Type == SIM7020_PDP_IP)
    {
        p_Params->IP = SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".") + "." +
                       SIM70XX_Tools_SubstringSplitErase(&Response, ".");
        p_Params->Subnet = Response;
    }

    SIM70XX_LOGD(TAG, "Type: %u", p_Params->Type);
    SIM70XX_LOGD(TAG, "IP: %s", p_Params->IP.c_str());
    SIM70XX_LOGD(TAG, "Subnet: %s", p_Params->Subnet.c_str());
    SIM70XX_LOGD(TAG, "Operator: %s", p_Params->APN.c_str());
    SIM70XX_LOGD(TAG, "CID: %u", p_Params->CID);
    SIM70XX_LOGD(TAG, "Baerer: %u", p_Params->Baerer);

    return SIM70XX_ERR_OK;
}

#endif