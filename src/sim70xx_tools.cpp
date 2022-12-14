 /*
 * sim70xx_tools.cpp
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

#include <mbedtls/base64.h>

#include <cmath>
#include <stdio.h>

#include "sim70xx_tools.h"

#include "Core/Arch/ESP32/UART/sim70xx_uart.h"
#include "Core/Arch/ESP32/GPIO/sim70xx_gpio.h"
#include "Core/Arch/ESP32/Logging/sim70xx_logging.h"
#include "Core/Arch/ESP32/Watchdog/sim70xx_watchdog.h"

static const char* TAG = "SIM70XX_Tools";

void SIM70XX_Tools_Buf2Hex(const void* const p_Buffer, uint32_t Length, std::string* const p_Hex)
{
    char Buffer[3];

    if((p_Buffer == NULL) || (p_Hex == NULL))
    {
        return;
    }

    p_Hex->clear();

    for(uint32_t i = 0; i < Length; i++)
    {
        sprintf(Buffer, "%02x", ((uint8_t*)p_Buffer)[i]);
        p_Hex->append(std::string(Buffer));
    }
}

void SIM70XX_Tools_Hex2ASCII(std::string Hex, uint8_t* const p_Buffer)
{
    uint8_t Offset;
    uint8_t High = 0;

    if((p_Buffer == NULL) || ((Hex.size() % 2) != 0))
    {
        return;
    }

    Offset = 0;
    for(uint32_t i = 0; i < Hex.size(); i++)
    {
        uint8_t Temp = 0;

        if((Hex.at(i) >= '0') && (Hex.at(i) <= '9'))
        {
            Temp = (uint8_t)Hex.at(i) - 48;
        }
        else if((Hex.at(i) >= 'a') && (Hex.at(i) <= 'f'))
        {
            Temp = (uint8_t)Hex.at(i) - 'a' + 10;
        }
        else if((Hex.at(i) >= 'A') && (Hex.at(i) <= 'F'))
        {
            Temp = (uint8_t)Hex.at(i) - 'A' + 10;
        }

        // Save the high byte.
        if((i % 2) == 0)
        {
            High = Temp;
        }
        // Save the low byte and push the result to the string.
        else
        {
            *(p_Buffer + (Offset++)) = (High << 0x04) + Temp;
        }
    }
}

void SIM70XX_Tools_Hex2ASCII(std::string Hex, std::string* const p_Buffer)
{
    uint8_t High = 0;

    if((p_Buffer == NULL) || ((Hex.size() % 2) != 0))
    {
        return;
    }

    for(uint32_t i = 0; i < Hex.size(); i++)
    {
        uint8_t Temp = 0;

        if((Hex.at(i) >= '0') && (Hex.at(i) <= '9'))
        {
            Temp = (uint8_t)Hex.at(i) - 0x30;
        }
        else if((Hex.at(i) >= 'a') && (Hex.at(i) <= 'f'))
        {
            Temp = (uint8_t)Hex.at(i) - 'a' + 10;
        }
        else if((Hex.at(i) >= 'A') && (Hex.at(i) <= 'F'))
        {
            Temp = (uint8_t)Hex.at(i) - 'A' + 10;
        }

        // Save the high byte.
        if((i % 2) == 0)
        {
            High = Temp;
        }
        // Save the low byte and push the result to the string.
        else
        {
            p_Buffer->push_back((char)(int)(((High << 0x04) + Temp)));
        }
    }
}

bool SIM70XX_Tools_ToBase64(const void* const p_Buffer, uint32_t Length, std::string* const p_Base64)
{
    size_t EncodeLength;
    unsigned char* OutputBuffer = NULL;

    if((p_Buffer == NULL) || (p_Base64 == NULL))
    {
        return false;
    }

    p_Base64->clear();

    mbedtls_base64_encode(OutputBuffer, 0, &EncodeLength, (const unsigned char*)p_Buffer, Length);

    SIM70XX_LOGI(TAG, "Payload length: %u", Length);
    SIM70XX_LOGI(TAG, "Allocate %u bytes...", EncodeLength);
    OutputBuffer = (unsigned char*)malloc(sizeof(unsigned char) * EncodeLength);
    if(OutputBuffer == NULL)
    {
        SIM70XX_LOGE(TAG, "Can not allocate memory!");

        return false;
    }

    mbedtls_base64_encode(OutputBuffer, EncodeLength, &EncodeLength, (const unsigned char*)p_Buffer, Length);
    p_Base64->append(std::string((char*)OutputBuffer));

    free(OutputBuffer);

    return true;
}

bool SIM70XX_Tools_EnableModule(SIM70XX_UART_Conf_t& p_Config, uint8_t Cycles)
{
    bool Result;
    uint8_t Attempts;

    SIM70XX_GPIO_Init();
    SIM70XX_GPIO_SetPwrKey(true);

    Result = false;
    Attempts = 0;
    do
    {
        if(SIM70XX_Tools_isActive(p_Config))
        {
            SIM70XX_LOGI(TAG, "Module active!");

            Result = true;

            break;
        }
        else
        {
            SIM70XX_LOGI(TAG, "Module inactive. Try to enable module: %u / %u...", Attempts + 1, Cycles);
            SIM70XX_LOGI(TAG, "Power on module...");

            Attempts++;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    } while((Attempts < Cycles));

    SIM70XX_GPIO_SetPwrKey(false);

    return Result;
}

bool SIM70XX_Tools_DisableModule(SIM70XX_UART_Conf_t& p_Config)
{
    std::string Response;

    if(SIM70XX_UART_Init(p_Config) != SIM70XX_ERR_OK)
    {
        return false;
    }

    SIM70XX_GPIO_Init();
    SIM70XX_GPIO_SetPwrKey(true);

    do
    {
        Response = SIM70XX_UART_ReadStringUntil(p_Config);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    } while(Response.find("NORMAL POWER DOWN") != std::string::npos);

    return true;
}

bool SIM70XX_Tools_isActive(SIM70XX_UART_Conf_t& p_Config)
{
    bool Result;
    std::string Response;
    SIM70XX_Error_t Error;

    Error = SIM70XX_UART_Init(p_Config) | SIM70XX_UART_SendLine(p_Config, "AT");
    if(Error != SIM70XX_ERR_OK)
    {
        SIM70XX_LOGE(TAG, "Error: 0x%X", Error);

        Result = false;
        goto SIM70XX_Tools_isActive_Exit;
    }

    Result = false;
    SIM70XX_UART_ReadStringUntil(p_Config);
    Response = SIM70XX_UART_ReadStringUntil(p_Config);
    SIM70XX_LOGD(TAG, "Response: %s", Response.c_str());
    SIM70XX_LOGD(TAG, "Length: %u", Response.size());
    if(Response.find("OK") != std::string::npos)
    {
        Result = true;
    }

    if(SIM70XX_UART_Deinit(p_Config) != SIM70XX_ERR_OK)
    {
        Result = false;
    }

SIM70XX_Tools_isActive_Exit:

    return Result;
}

bool SIM70XX_Tools_ResetModule(SIM70XX_UART_Conf_t& p_Config)
{
    if(SIM70XX_Tools_DisableModule(p_Config) == false)
    {
        SIM70XX_LOGI(TAG, "Can not disable the module!");

		return false;
    }

    if(SIM70XX_Tools_EnableModule(p_Config) == false)
    {
        SIM70XX_LOGE(TAG, "Can not enable module!");

		return false;
    }

    return true;
}

SIM70XX_Error_t SIM70XX_Tools_DisableEcho(SIM70XX_UART_Conf_t& p_Config)
{
    std::string Response;

    if(p_Config.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_WDT_PAUSE_TASK(p_Config.TaskHandle);

    // Check if echo mode is enabled. With echo mode enable the answer will have the following format:
    //  AT<CR><LF>
    //  OK<CR><LF><CR><LF>
    SIM70XX_UART_SendLine(p_Config, "AT");
    Response = SIM70XX_UART_ReadStringUntil(p_Config);
    SIM70XX_UART_ReadStringUntil(p_Config);
    if(Response.find("AT") != std::string::npos)
    {
        SIM70XX_LOGI(TAG, "Echo mode enabled. Disable echo mode...");

        // Disable echo mode.
        SIM70XX_UART_SendLine(p_Config, "ATE0");
        SIM70XX_UART_ReadStringUntil(p_Config);
        SIM70XX_UART_ReadStringUntil(p_Config);
    }
    else
    {
        SIM70XX_LOGI(TAG, "Echo mode disabled...");
    }

    SIM70XX_WDT_CONTINUE_TASK(p_Config.TaskHandle);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM70XX_Tools_SetBaudrate(SIM70XX_UART_Conf_t& p_Config, SIM70XX_Baud_t Old, SIM70XX_Baud_t New)
{
    std::string Status;

    // TODO:

/*
    // Initialize the serial interface with the old baudrate.
    vTaskSuspend(p_Device.UART.TaskHandle);
    p_Device.UART.Baudrate = Old;
    SIM70XX_ERROR_CHECK(SIM70XX_UART_Deinit(p_Device.UART));
    SIM70XX_ERROR_CHECK(SIM70XX_UART_Init(p_Device.UART));
    SIM70XX_WDT_CONTINUE_TASK(p_Device.UART.TaskHandle);

    // Set the new baudrate.
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT_IPR_W(New);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Status));

    if(Status.find("OK") == std::string::npos)
    {
        SIM70XX_LOGE(TAG, "Can not enable new baudrate!");

        // Switch back to the old baudrate.
        SIM70XX_WDT_PAUSE_TASK(p_Device.UART.TaskHandle);
        p_Device.UART.Baudrate = Old;
        SIM70XX_ERROR_CHECK(SIM70XX_UART_Deinit(p_Device.UART));
        SIM70XX_ERROR_CHECK(SIM70XX_UART_Init(p_Device.UART));
        SIM70XX_WDT_CONTINUE_TASK(p_Device.UART.TaskHandle);

        return SIM70XX_ERR_FAIL;
    }

    SIM70XX_LOGI(TAG, "New baudrate enabled. Reinitialize the interface!");

    // Reinitialize the interface with the new baudrate.
    SIM70XX_WDT_PAUSE_TASK(p_Device.UART.TaskHandle);
    p_Device.UART.Baudrate = New;
    SIM70XX_ERROR_CHECK(SIM70XX_UART_Deinit(p_Device.UART));
    SIM70XX_ERROR_CHECK(SIM70XX_UART_Init(p_Device.UART));
    SIM70XX_WDT_CONTINUE_TASK(p_Device.UART.TaskHandle);
*/
    return SIM70XX_ERR_OK;
}

std::string SIM70XX_Tools_SubstringSplitErase(std::string* p_Input, std::string Delimiter)
{
    size_t Index;
    std::string Result = std::string();

    assert(p_Input);

    Index = p_Input->find(Delimiter);
    if(Index != std::string::npos)
    {
        Result = p_Input->substr(0, Index);
        p_Input->erase(0, Index + 1);
    }

    return Result;
}

void SIM70XX_Tools_StringRemove(std::string* p_Input, std::string Remove)
{
    size_t Index;

    assert(p_Input);

    do
    {
        Index = p_Input->find(Remove);
        if(Index != std::string::npos)
        {
            p_Input->replace(Index, std::string(Remove).size(), "");
        }
    } while(Index != std::string::npos);
}

uint32_t SIM70XX_Tools_StringToUnsigned(std::string Input)
{
    #if __cpp_exceptions
        try
        {
            return (uint32_t)std::stoi(Input);
        }
        catch(...)
        {
            return INT_MAX;
        }
    #else
        if(Input.size() > 0)
        {
            return (uint32_t)std::stoi(Input);
        }
        else
        {
            return 0;
        }
    #endif
}

int32_t SIM70XX_Tools_StringToSigned(std::string Input)
{
    #if __cpp_exceptions
        try
        {
            return (int32_t)std::stoi(Input);
        }
        catch(...)
        {
            return INT_MAX;
        }
    #else
        if(Input.size() > 0)
        {
            return (int32_t)std::stoi(Input);
        }
        else
        {
            return 0;
        }
    #endif
}

float SIM70XX_Tools_StringToFloat(std::string Input)
{
    #if __cpp_exceptions
        try
        {
            return std::stof(Input);
        }
        catch(...)
        {
            return std::nanf("");
        }
    #else
        if(Input.size() > 0)
        {
            return std::stof(Input);
        }
        else
        {
            return std::nanf("");
        }
    #endif
}