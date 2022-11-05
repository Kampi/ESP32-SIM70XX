 /*
 * sim7020_nvram.cpp
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

#if((CONFIG_SIMXX_DEV == 7020) && (defined CONFIG_SIM70XX_DRIVER_WITH_NVRAM))

#include <esp_log.h>

#include "sim7020.h"
#include "sim7020_nvram.h"
#include "../../Core/Arch/ESP32/UART/sim70xx_uart.h"
#include "../../Core/Queue/sim70xx_queue.h"
#include "../../Core/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7020_NVRAM";

SIM70XX_Error_t SIM7020_NVRAM_Write(SIM7020_t& p_Device, std::string Key, const std::string Data, SIM7020_NVRAM_Error_t* p_Error)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM7020_NVRAM_Error_t Error;

    if((Key.size() < 1) || (Key.size() > 20) || (Data.size() < 1) || (Data.size() > 1024))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    ESP_LOGD(TAG, "Writing to NVRAM...");

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CNVMW(Key, Data, Data.size());
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    Error = (SIM7020_NVRAM_Error_t)SIM70XX_Tools_StringToSigned(Response);

    if(p_Error != NULL)
    {
        *p_Error = Error;
    }

    if(Error != SIM7020_NVRAM_ERR_OK)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_NVRAM_Write(SIM7020_t& p_Device, std::string Key, const uint8_t* p_Buffer, uint16_t Length, SIM7020_NVRAM_Error_t* p_Error)
{
    std::string HexString;

    if(p_Buffer == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    SIM70XX_Tools_Buf2Hex(p_Buffer, Length, &HexString);

    return SIM7020_NVRAM_Write(p_Device, Key, HexString, p_Error);
}

SIM70XX_Error_t SIM7020_NVRAM_Read(SIM7020_t& p_Device, std::string Key, std::string* p_Payload, SIM7020_NVRAM_Error_t* p_Error)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM7020_NVRAM_Error_t Error;

    if((p_Payload == NULL) || (Key.size() < 1) || (Key.size() > 20))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    ESP_LOGD(TAG, "Reading from NVRAM...");

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CNVMR(Key);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    // Filter out the error code.
    Error = (SIM7020_NVRAM_Error_t)SIM70XX_Tools_StringToSigned(SIM70XX_Tools_SubstringSplitErase(&Response));
    if(p_Error != NULL)
    {
        *p_Error = Error;
    }

    if(Error != SIM7020_NVRAM_ERR_OK)
    {
        return SIM70XX_ERR_FAIL;
    }

    // Filter out the key.
    SIM70XX_Tools_SubstringSplitErase(&Response);

    // Filter out the length.
    SIM70XX_Tools_SubstringSplitErase(&Response);

    // Filter out the payload.
    *p_Payload = Response;

    ESP_LOGD(TAG, "Error: %i", Error);
    ESP_LOGD(TAG, "Length: %i", p_Payload->length());
    ESP_LOGD(TAG, "Payload: %s", p_Payload->c_str());

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_NVRAM_Erase(SIM7020_t& p_Device, std::string Key, SIM7020_NVRAM_Error_t* p_Error)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM7020_NVRAM_Error_t Error;

    if((Key.size() < 1) || (Key.size() > 20))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CNVMIVD(Key);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    // Filter out the error code.
    Error = (SIM7020_NVRAM_Error_t)SIM70XX_Tools_StringToSigned(Response);

    if(p_Error != NULL)
    {
        *p_Error = Error;
    }

    if(Error != SIM7020_NVRAM_ERR_OK)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_NVRAM_GetKeys(SIM7020_t& p_Device, std::vector<std::string>* p_Keys)
{
    SIM70XX_Error_t Error = SIM70XX_ERR_OK;
    SIM70XX_TxCmd_t Command;

    if(p_Keys == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    p_Keys->clear();

    // We have to pause the UART thread here and receive the response line by line, because we don´t know the exakt number of keys.
    vTaskSuspend(p_Device.Internal.TaskHandle);

    // Transmit the command.
    Command = SIM7020_AT_CNVMGET;
    SIM70XX_UART_SendLine(p_Device.UART, Command.Command);

    // Get the responses.
    do
    {
        std::string Response;

        Response = SIM70XX_UART_ReadStringUntil(p_Device.UART);

        // We have received something.
        if(Response.size() > 2)
        {
            ESP_LOGD(TAG, "Response: %s", Response.c_str());

            // OK received. Abort here.
            if((Response.find("OK") != std::string::npos) && (Response.size() < 4))
            {
                break;
            }
            // We got a valid entry here.
            else if(Response.find("+CNVMGET:") != std::string::npos)
            {
                // Remove the command and the index.
                SIM70XX_Tools_SubstringSplitErase(&Response);

                // Remove the group name.
                SIM70XX_Tools_SubstringSplitErase(&Response);

                // Replace the quotations.
                Response.erase(std::remove(Response.begin(), Response.end(), '\"'), Response.end());

                ESP_LOGD(TAG, "Key: %s", Response.c_str());

                p_Keys->push_back(Response);
            }
            // Unknown response. Abort here.
            else
            {
                Error = SIM70XX_ERR_FAIL;
                break;
            }
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    } while(true);
    vTaskResume(p_Device.Internal.TaskHandle);

    return Error;
}

#endif