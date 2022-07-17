 /*
 * sim7080_fs.cpp
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

#if((CONFIG_SIMXX_DEV == 7080) && (defined CONFIG_SIM70XX_DRIVER_WITH_FS))

#include <esp_log.h>

#include "sim7080.h"
#include "sim7080_fs.h"
#include "sim70xx_tools.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7080_FS";

SIM70XX_Error_t SIM7080_FS_Delete(SIM7080_t* const p_Device, std::string File)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    return SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue);
}

SIM70XX_Error_t SIM7080_FS_Rename(SIM7080_t* const p_Device, std::string Old, std::string New)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    return SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue);
}

SIM70XX_Error_t SIM7080_FS_GetFree(SIM7080_t* const p_Device, uint32_t* const p_Free)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_Free == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CFSINIT;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CFSGFRS;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    *p_Free = std::stoi(Response);

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CFSTERM;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue);
}

#endif