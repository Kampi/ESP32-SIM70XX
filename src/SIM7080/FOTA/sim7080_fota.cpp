 /*
 * sim7080_fota.cpp
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

#if((CONFIG_SIMXX_DEV == 7080) && (defined CONFIG_SIM70XX_DRIVER_WITH_FOTA))

#include "sim7080.h"
#include "sim7080_fota.h"

#include "../../Core/Queue/sim70xx_queue.h"
#include "../../Core/Commands/sim70xx_commands.h"

#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7080_FOTA";

SIM70XX_Error_t SIM7080_FOTA_HTTP_Init(SIM7080_t& p_Device, std::string URL, std::string Path, uint16_t Timeout, uint8_t Retries, SIM7080_FOTA_HTTP_Status_t* p_Status)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM7080_FOTA_HTTP_Status_t Status;

    if((URL.size() == 0) || (Path.size() == 0) || (Timeout < 10) || (Timeout > 1000) || (Retries < 5) || (Retries > 100))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    Status = SIM7080_FOTA_HTTP_BAD_REQ;

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_HTTPTOFS(URL, Path, Timeout, Retries);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    if(p_Status != NULL)
    {
        *p_Status = Status;
    }

    p_Device.FOTA.isPending = true;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_FOTA_Execute(SIM7080_t& p_Device)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device.FOTA.isPending == false)
    {
        return SIM70XX_ERR_FOTA_NOT_ACTIVE;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CFOTA;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    p_Device.FOTA.isPending = false;

    return SIM70XX_ERR_OK;
}

#endif