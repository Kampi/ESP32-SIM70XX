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

#include <esp_log.h>
#include <esp_timer.h>

#include <mbedtls/base64.h>

#include <stdio.h>

#include "sim70xx_tools.h"
#include "Private/UART/sim70xx_uart.h"

static const char* TAG = "SIM70XX_Tools";

void SIM70XX_Tools_ASCII2Hex(const void* const p_Buffer, uint32_t Length, std::string* const p_Hex)
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
    uint8_t Low = 0;
    uint8_t High = 0;

    if((p_Buffer == NULL) || ((Hex.length() % 2) != 0))
    {
        return;
    }

    Offset = 0;
    for(uint32_t i = 0; i < Hex.length(); i++)
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
            Low = Temp;

            *(p_Buffer + (Offset++)) = (High << 0x04) + Low;
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

    // Get the size for memory allocation first.
    mbedtls_base64_encode(OutputBuffer, 0, &EncodeLength, (const unsigned char*)p_Buffer, Length);

    ESP_LOGI(TAG, "Payload length: %u", Length);
    ESP_LOGI(TAG, "Allocate %u bytes...", EncodeLength);
    OutputBuffer = (unsigned char*)malloc(sizeof(unsigned char) * EncodeLength);
    if(OutputBuffer == NULL)
    {
        ESP_LOGE(TAG, "Can not allocate memory!");

        return false;
    }

    // Encode the data.
    mbedtls_base64_encode(OutputBuffer, EncodeLength, &EncodeLength, (const unsigned char*)p_Buffer, Length);
    p_Base64->append(std::string((char*)OutputBuffer));

    free(OutputBuffer);

    return true;
}

unsigned long IRAM_ATTR SIM70XX_Tools_GetmsTimer(void)
{
    return (unsigned long)(esp_timer_get_time() / 1000ULL);
}

bool SIM70XX_Tools_isActive(SIM70XX_UART_Conf_t& p_Config)
{
    std::string Response;

    if((SIM70XX_UART_Init(p_Config) != SIM70XX_ERR_OK) || (SIM70XX_UART_SendCommand(p_Config, "AT") != SIM70XX_ERR_OK))
    {
        return false;
    }

    SIM70XX_UART_ReadStringUntil(p_Config, '\n');
    Response = SIM70XX_UART_ReadStringUntil(p_Config, '\n');
    if(SIM70XX_UART_Deinit(p_Config) != SIM70XX_ERR_OK)
    {
        return false;
    }

    ESP_LOGI(TAG, "Response from 'AT': %s", Response.c_str());

    if(Response.find("OK") != std::string::npos)
    {
        return true;
    }

    return false;
}

SIM70XX_Error_t SIM70XX_Tools_DisableEcho(SIM70XX_UART_Conf_t& p_Config)
{
    std::string Response;

    if(p_Config.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    // Check if echo mode is enabled. With echo mode enable the answer will have the following format:
    //  AT<CR><LF>
    //  OK<CR><LF><CR><LF>
    SIM70XX_UART_SendCommand(p_Config, "AT");
    Response = SIM70XX_UART_ReadStringUntil(p_Config, '\n');
    SIM70XX_UART_ReadStringUntil(p_Config, '\n');
    if(Response.find("AT") != std::string::npos)
    {
        ESP_LOGI(TAG, "Echo mode enabled. Disable echo mode...");

        // Disable echo mode.
        SIM70XX_UART_SendCommand(p_Config, "ATE0");
        SIM70XX_UART_ReadStringUntil(p_Config, '\n');
        SIM70XX_UART_ReadStringUntil(p_Config, '\n');
    }
    else
    {
        ESP_LOGI(TAG, "Echo mode disabled...");
    }

    return SIM70XX_ERR_OK;
}

std::string SIM70XX_Tools_SubstringSplitErase(std::string* p_Input, std::string Delimiter)
{
    size_t Index;
    std::string Result;

    assert(p_Input);

    Index = p_Input->find(",");
    Result = p_Input->substr(0, Index);
    p_Input->erase(0, Index + 1);

    return Result;
}