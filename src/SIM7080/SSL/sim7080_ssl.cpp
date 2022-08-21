 /*
 * sim7080_ssl.cpp
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

#if((CONFIG_SIMXX_DEV == 7080) && (defined CONFIG_SIM70XX_DRIVER_WITH_SSL))

#include <esp_log.h>

#include "sim7080.h"
#include "sim7080_fs.h"
#include "sim7080_ssl.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7080_SSL";

/** @brief              Execute a SSL convert command.
 *  @param p_Device     SIM7080 device object
 *  @param CommandStr   Command string to be executed
 *  @return             SIM70XX_ERR_OK when successful
 */
static SIM70XX_Error_t SIM7080_SSL_Convert(SIM7080_t& p_Device, std::string CommandStr)
{
    SIM70XX_TxCmd_t* Command;

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CFSINIT;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CSSLCFG(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CFSTERM;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

SIM70XX_Error_t SIM7080_SSL_Configure(SIM7080_t& p_Device)
{
    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    // TODO

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_SSL_ImportRoot(SIM7080_t& p_Device, SIM7080_FS_Path_t Path, SIM7080_SSL_Type_t Type, SIM7080_SSL_File_t RootCA, uint8_t CID)
{
    std::string CommandStr;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_ERROR_CHECK(SIM7080_FS_Write(p_Device, Path, RootCA.Name, RootCA.p_Data, RootCA.Size, false, 10000));

    CommandStr = "AT+CSSLCFG=\"CONVERT\"," + std::to_string(Type) + "," + RootCA.Name;

    SIM70XX_ERROR_CHECK(SIM7080_SSL_Convert(p_Device, CommandStr));

    return SIM7080_FS_Delete(p_Device, Path, RootCA.Name);
}

SIM70XX_Error_t SIM7080_SSL_ImportCert(SIM7080_t& p_Device, SIM7080_FS_Path_t Path, SIM7080_SSL_Type_t Type, SIM7080_SSL_File_t Client_Cer, SIM7080_SSL_File_t Client_Key, std::string Password, uint8_t CID)
{
    std::string CommandStr;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_ERROR_CHECK(SIM7080_FS_Write(p_Device, Path, Client_Cer.Name, Client_Cer.p_Data, Client_Cer.Size));
    SIM70XX_ERROR_CHECK(SIM7080_FS_Write(p_Device, Path, Client_Key.Name, Client_Key.p_Data, Client_Key.Size));

    if(Password.size() > 0)
    {
        CommandStr = "AT+CSSLCFG=\"CONVERT\"," + std::to_string(Type) + "," + Client_Cer.Name + "," + Client_Key.Name + ",\"" + Password + "\"";
    }
    else
    {
        CommandStr = "AT+CSSLCFG=\"CONVERT\"," + std::to_string(Type) + "," + Client_Cer.Name + "," + Client_Key.Name;
    }

    SIM70XX_ERROR_CHECK(SIM7080_SSL_Convert(p_Device, CommandStr));
    SIM70XX_ERROR_CHECK(SIM7080_FS_Delete(p_Device, Path, Client_Cer.Name));

    return SIM7080_FS_Delete(p_Device, Path, Client_Key.Name);
}

#endif