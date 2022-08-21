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
#include "../../Private/UART/sim70xx_uart.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

/** @brief Maximum file size.
 */
#define SIM7080_FS_MAX_FILE_SIZE                            10240

static const char* TAG = "SIM7080_FS";

/** @brief          Initialize the file system.
 *  @param p_Device SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
static SIM70XX_Error_t SIM7080_FS_Init(SIM7080_t& p_Device)
{
    SIM70XX_TxCmd_t* Command;

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CFSINIT;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

/** @brief          Deinitialize the file system.
 *  @param p_Device SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
static SIM70XX_Error_t SIM7080_FS_Deinit(SIM7080_t& p_Device)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CFSTERM;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

SIM70XX_Error_t SIM7080_FS_GetFileSize(SIM7080_t& p_Device, SIM7080_FS_Path_t Path, std::string Name, size_t* p_Size)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Size == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_ERROR_CHECK(SIM7080_FS_Init(p_Device));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CFSGFIS(Path, Name);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    *p_Size = (size_t)std::stoi(Response);

    return SIM7080_FS_Deinit(p_Device);
}

SIM70XX_Error_t SIM7080_FS_Write(SIM7080_t& p_Device, SIM7080_FS_Path_t Path, std::string Name, const void* const p_Buffer, uint16_t Length, bool Append, uint16_t Timeout)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM70XX_Error_t Error = SIM70XX_ERR_OK;

    if((Name.size() > 230) || ((p_Buffer == NULL) && (Length > 0)) || (Length > SIM7080_FS_MAX_FILE_SIZE) || (Timeout < 100) || (Timeout > 10000))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device.FS.Free < Length)
    {
        return SIM70XX_ERR_NO_MEM;
    }

    SIM70XX_ERROR_CHECK(SIM7080_FS_Init(p_Device));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CFSWFILE(Path, Name, Append, Length, Timeout);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, NULL, &Response);
    if(Response.find("DOWNLOAD") == std::string::npos)
    {
        Error = SIM70XX_ERR_FAIL;
        goto SIM7080_FS_Write_Exit;
    }

    vTaskSuspend(p_Device.Internal.TaskHandle);
    SIM70XX_UART_Send(p_Device.UART, p_Buffer, Length);
    SIM70XX_UART_ReadStringUntil(p_Device.UART);
    Response = SIM70XX_UART_ReadStringUntil(p_Device.UART);
    vTaskResume(p_Device.Internal.TaskHandle);
    if(Response.find("OK") == std::string::npos)
    {
        Error = SIM70XX_ERR_FAIL;
        goto SIM7080_FS_Write_Exit;
    }

    ESP_LOGD(TAG, "Response: %s", Response.c_str());

    p_Device.FS.Free -= Length;

SIM7080_FS_Write_Exit:
    SIM7080_FS_Deinit(p_Device);

    return Error;
}

SIM70XX_Error_t SIM7080_FS_Read(SIM7080_t& p_Device, SIM7080_FS_Path_t Path, std::string Name, void* const p_Buffer, uint16_t Length, bool UsePosition, uint16_t Position)
{
    uint8_t* Buffer_Temp = (uint8_t*)p_Buffer;
    uint16_t BytesRead;
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if((Name.size() > 230) || ((p_Buffer == NULL) && (Length > 0)) || (Length > SIM7080_FS_MAX_FILE_SIZE) || ((UsePosition == true) && (Position > SIM7080_FS_MAX_FILE_SIZE)))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    
    SIM70XX_ERROR_CHECK(SIM7080_FS_Init(p_Device));

    SIM70XX_CREATE_CMD(Command);
    if(UsePosition == false)
    {
        *Command = SIM7080_AT_CFSRFILE(Path, Name, UsePosition, Length, 0);
    }
    else
    {
        *Command = SIM7080_AT_CFSRFILE(Path, Name, UsePosition, Length, Position);
    }

    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, NULL, &Response);
    if(Response.find("+CFSRFILE:") == std::string::npos)
    {
        SIM7080_FS_Deinit(p_Device);

        return SIM70XX_ERR_FAIL;
    }

    vTaskSuspend(p_Device.Internal.TaskHandle);
    BytesRead = 0;
    do
    {
        int c;

        c = SIM70XX_UART_Read(p_Device.UART);
        if(c != -1)
        {
            *Buffer_Temp++ = c;
            BytesRead++;
        }
    } while(BytesRead < Length);

    // Read the trailing "OK".
    SIM70XX_UART_ReadStringUntil(p_Device.UART);
    SIM70XX_UART_ReadStringUntil(p_Device.UART);
    Response = SIM70XX_UART_ReadStringUntil(p_Device.UART);

    vTaskResume(p_Device.Internal.TaskHandle);

    ESP_LOGI(TAG, "Response: %s", Response.c_str());

    if(Response.find("OK") == std::string::npos)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM7080_FS_Deinit(p_Device);
}

SIM70XX_Error_t SIM7080_FS_Delete(SIM7080_t& p_Device, SIM7080_FS_Path_t Path, std::string Name)
{
    uint32_t Size;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_ERROR_CHECK(SIM7080_FS_GetFileSize(p_Device, Path, Name, &Size))

    SIM70XX_ERROR_CHECK(SIM7080_FS_Init(p_Device));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CFSDFILE(Path, Name);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    p_Device.FS.Free += Size;

    return SIM7080_FS_Deinit(p_Device);
}

SIM70XX_Error_t SIM7080_FS_Rename(SIM7080_t& p_Device, SIM7080_FS_Path_t Path, std::string Old, std::string New)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_ERROR_CHECK(SIM7080_FS_Init(p_Device));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CFSREN(Path, Old, New);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    return SIM7080_FS_Deinit(p_Device);
}

SIM70XX_Error_t SIM7080_FS_GetFree(SIM7080_t& p_Device, uint32_t* const p_Free)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Free == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_ERROR_CHECK(SIM7080_FS_Init(p_Device));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CFSGFRS;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    p_Device.FS.Free = std::stoi(Response);
    *p_Free = p_Device.FS.Free;

    return SIM7080_FS_Deinit(p_Device);
}

#endif