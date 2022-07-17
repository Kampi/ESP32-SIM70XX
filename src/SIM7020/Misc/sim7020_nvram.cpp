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
#include "sim70xx_tools.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7020_NVRAM";

SIM70XX_Error_t SIM7020_NVRAM_Write(SIM7020_t* const p_Device, std::string Key, const std::string Data, SIM7020_NVRAM_Error_t* p_Error)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM7020_NVRAM_Error_t Error;

    if((p_Device == NULL) || (Key.length() < 1) || (Key.length() > 20) || (Data.length() < 1) || (Data.length() > 1024))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    ESP_LOGD(TAG, "Writing to NVRAM...");

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CNVMW(Key, Data, Data.length());
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    Error = (SIM7020_NVRAM_Error_t)std::stoi(Response);

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

SIM70XX_Error_t SIM7020_NVRAM_Write(SIM7020_t* const p_Device, std::string Key, const uint8_t* p_Buffer, uint16_t Length, SIM7020_NVRAM_Error_t* p_Error)
{
    std::string HexString;

    SIM70XX_Tools_ASCII2Hex(p_Buffer, Length, &HexString);

    return SIM7020_NVRAM_Write(p_Device, Key, HexString, p_Error);
}

SIM70XX_Error_t SIM7020_NVRAM_Read(SIM7020_t* const p_Device, std::string Key, std::string* p_Payload, SIM7020_NVRAM_Error_t* p_Error)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM7020_NVRAM_Error_t Error;

    if((p_Device == NULL) || (p_Payload == NULL) || (Key.length() < 1) || (Key.length() > 20))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    ESP_LOGD(TAG, "Reading from NVRAM...");

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CNVMR(Key);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    // Filter out the error code.
    Error = (SIM7020_NVRAM_Error_t)std::stoi(Response.substr(0, Response.find(",")));
    Response.erase(0, Response.find(",") + 1);

    if(p_Error != NULL)
    {
        *p_Error = Error;
    }

    if(Error != SIM7020_NVRAM_ERR_OK)
    {
        return SIM70XX_ERR_FAIL;
    }

    // Filter out the key.
    Response.substr(0, Response.find(","));
    Response.erase(0, Response.find(",") + 1);

    // Filter out the length.
    Response.substr(0, Response.find(","));
    Response.erase(0, Response.find(",") + 1);

    // Filter out the payload.
    *p_Payload = Response.substr(0);
    p_Payload->replace(p_Payload->begin(), p_Payload->end(), "\"", "");

    ESP_LOGD(TAG, "Error: %i", Error);
    ESP_LOGD(TAG, "Length: %i", p_Payload->length());
    ESP_LOGD(TAG, "Payload: %s", p_Payload->c_str());

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_NVRAM_Erase(SIM7020_t* const p_Device, std::string Key, SIM7020_NVRAM_Error_t* p_Error)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM7020_NVRAM_Error_t Error;

    if((p_Device == NULL) || (Key.length() < 1) || (Key.length() > 20))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CNVMIVD(Key);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    // Filter out the error code.
    Error = (SIM7020_NVRAM_Error_t)std::stoi(Response.substr(0, Response.find(",")));

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

SIM70XX_Error_t SIM7020_NVRAM_GetKeys(SIM7020_t* const p_Device, std::vector<std::string>* p_Keys)
{
    int32_t Index;
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_Keys == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    p_Keys->clear();

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CNVMGET;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    // Split the response into the different keys.
    do
    {
        std::string Key;

        Index = Response.find('\n');

        Key = Response.substr(0, Index);

        if(Key.length() < 1)
        {
            break;
        }

        Response.erase(0, Index + 1);
        Key.replace(Key.begin(), Key.end(), "\n", "");

        Key = Key.substr(Key.find_last_of(",") + 1);
        Key.replace(Key.begin(), Key.end(), "\"", "");

        p_Keys->push_back(Key);
        ESP_LOGI(TAG, "Key: %s", Key.c_str());
    } while(true);

    return SIM70XX_ERR_OK;
}

#endif