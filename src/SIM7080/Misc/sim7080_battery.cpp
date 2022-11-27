 /*
 * sim7080_battery.cpp
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

#include "../../Core/Queue/sim70xx_queue.h"
#include "../../Core/Commands/sim70xx_commands.h"

#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

SIM70XX_Error_t SIM7080_Info_ReadBattery(SIM7080_t& p_Device, SIM7080_ChargeState_t* const p_Charge)
{
    return SIM7080_Info_ReadBattery(p_Device, p_Charge, NULL, NULL);
}

SIM70XX_Error_t SIM7080_Info_ReadBattery(SIM7080_t& p_Device, uint8_t* const p_Battery)
{
    return SIM7080_Info_ReadBattery(p_Device, NULL, p_Battery, NULL);
}

SIM70XX_Error_t SIM7080_Info_ReadBattery(SIM7080_t& p_Device, uint16_t* const p_Voltage)
{
    return SIM7080_Info_ReadBattery(p_Device, NULL, NULL, p_Voltage);
}

SIM70XX_Error_t SIM7080_Info_ReadBattery(SIM7080_t& p_Device, SIM7080_ChargeState_t* const p_Charge, uint8_t* const p_Battery, uint16_t* const p_Voltage)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM7080_ChargeState_t Charge;
    uint8_t Battery;
    uint16_t Voltage;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CBC
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    Charge = (SIM7080_ChargeState_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response));
    Battery = (uint8_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response));
    Voltage = (uint16_t)SIM70XX_Tools_StringToUnsigned(Response);

    if(p_Charge != NULL)
    {
        *p_Charge = Charge;
    }

    if(p_Battery != NULL)
    {
        *p_Battery = Battery;
    }

    if(p_Voltage != NULL)
    {
        *p_Voltage = Voltage;
    }

    return SIM70XX_ERR_OK;
}

#endif