 /*
 * sim70xx_uart.cpp
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

#include <esp_err.h>
#include <esp_task_wdt.h>

#include <driver/uart.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "sim70xx_uart.h"
#include "sim70xx_tools.h"

#include "../Timer/sim70xx_timer.h"
#include "../Logging/sim70xx_logging.h"
#include "../Watchdog/sim70xx_watchdog.h"

#include <sdkconfig.h>

#ifndef CONFIG_SIM70XX_UART_BUFFER_SIZE
    #define CONFIG_SIM70XX_UART_BUFFER_SIZE                 256
#endif

static uart_config_t _SIM70XX_UART_Config = {
    .baud_rate              = 0,
    .data_bits              = UART_DATA_8_BITS,
    .parity                 = UART_PARITY_DISABLE,
    .stop_bits              = UART_STOP_BITS_1,
    .flow_ctrl              = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh    = 0,
#ifdef CONFIG_PM_ENABLE
    .source_clk             = UART_SCLK_REF_TICK,
#else
    .source_clk             = UART_SCLK_APB,
#endif
};

static const char* TAG = "SIM70XX_UART";

/** @brief          Get a character from the UART FIFO.
 *  @param p_Config Pointer to SIM70XX UART configuration object
 *  @return         Received character
 */
static uint8_t SIM7020_UART_Get(SIM70XX_UART_Conf_t& p_Config)
{
    uint8_t c = 0;

    if(xSemaphoreTake(p_Config.Lock, 10 / portTICK_PERIOD_MS) == pdTRUE)
    {
        if(uart_read_bytes(p_Config.Interface, &c, sizeof(uint8_t), 20 / portTICK_RATE_MS) == 0)
        {
            c = 0;
        }
        xSemaphoreGive(p_Config.Lock);
    }

    return c;
}

/** @brief          Wait for a character and read it.
 *  @param p_Config Pointer to SIM70XX UART configuration object
 *  @param Timeout  (Optional) Read timeout in milliseconds
 *  @return         Received character or -1 when no character received
 */
static int SIM7020_UART_TimedRead(SIM70XX_UART_Conf_t& p_Config, uint32_t Timeout = 1000)
{
    int c;
    unsigned long Now;

    Now = SIM70XX_Timer_GetMilliseconds();
    do
    {
        SIM70XX_WDT_Reset();
        c = SIM70XX_UART_Read(p_Config);
        if(c >= 0)
        {
            return c;
        }
    } while((SIM70XX_Timer_GetMilliseconds() - Now) < Timeout);

    return -1;
}

SIM70XX_Error_t SIM70XX_UART_Init(SIM70XX_UART_Conf_t& p_Config)
{
    int RTS;
    int CTS;
    uint8_t Flags;

    if(p_Config.isInitialized)
    {
        SIM70XX_ERROR_CHECK(SIM70XX_UART_Deinit(p_Config));
    }

    Flags = 0;
    p_Config.isInitialized = false;

    _SIM70XX_UART_Config.baud_rate = p_Config.Baudrate;

    p_Config.Lock = xSemaphoreCreateMutex();
    if(p_Config.Lock == NULL)
    {
        return SIM70XX_ERR_NO_MEM;
    }

    #ifdef CONFIG_SIM70XX_UART_USE_RTS
        if(GPIO_IS_VALID_GPIO(CONFIG_SIM70XX_UART_RTS_PIN))
        {
            RTS = CONFIG_SIM70XX_UART_RTS_PIN;
        }
        else
        {
            SIM70XX_LOGE(TAG, "Invalid GPIO for RTS!");

            return SIM70XX_ERR_INVALID_ARG;
        }
    #else
        RTS = UART_PIN_NO_CHANGE;
    #endif

    #ifdef CONFIG_SIM70XX_UART_USE_CTS
        if(GPIO_IS_VALID_GPIO(CONFIG_SIM70XX_UART_CTS_PIN))
        {
            RTS = CONFIG_SIM70XX_UART_CTS_PIN;
        }
        else
        {
            SIM70XX_LOGE(TAG, "Invalid GPIO for CTS!");

            return SIM70XX_ERR_INVALID_ARG;
        }
    #else
        CTS = UART_PIN_NO_CHANGE;
    #endif

    #ifdef CONFIG_SIM70XX_UART_IRAM
        Flags = ESP_INTR_FLAG_IRAM;
    #endif

    if(xSemaphoreTake(p_Config.Lock, 10 / portTICK_PERIOD_MS) == pdTRUE)
    {
        if((uart_driver_install(p_Config.Interface, CONFIG_SIM70XX_UART_BUFFER_SIZE * 2, 0, 0, NULL, Flags) ||
            uart_param_config(p_Config.Interface, &_SIM70XX_UART_Config) ||
            uart_set_pin(p_Config.Interface, p_Config.Tx, p_Config.Rx, RTS, CTS) ||
            uart_flush(p_Config.Interface)
           ) != ESP_OK)
        {
            xSemaphoreGive(p_Config.Lock);

            return SIM70XX_ERR_NOT_INITIALIZED;
        }
        xSemaphoreGive(p_Config.Lock);

        SIM70XX_UART_Flush(p_Config);

        SIM70XX_LOGD(TAG, "UART initialized...");

        if(p_Config.TaskHandle != NULL)
        {
            SIM70XX_WDT_AddHandle(p_Config.TaskHandle);
        }

        p_Config.isInitialized = true;

        return SIM70XX_ERR_OK;
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM70XX_UART_Deinit(SIM70XX_UART_Conf_t& p_Config)
{
    if(p_Config.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    if(xSemaphoreTake(p_Config.Lock, 10 / portTICK_PERIOD_MS) == pdTRUE)
    {
        if(uart_is_driver_installed(p_Config.Interface))
        {
            uart_flush(p_Config.Interface);
            uart_driver_delete(p_Config.Interface);
            gpio_reset_pin(p_Config.Rx);
            gpio_reset_pin(p_Config.Tx);
        }
        xSemaphoreGive(p_Config.Lock);

        vSemaphoreDelete(p_Config.Lock);

        SIM70XX_LOGD(TAG, "UART deinitialized...");

        p_Config.isInitialized = false;

        if(p_Config.TaskHandle != NULL)
        {
            SIM70XX_WDT_RemoveHanndle(p_Config.TaskHandle);
        }

        return SIM70XX_ERR_OK;
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM70XX_UART_Send(SIM70XX_UART_Conf_t& p_Config, const void* p_Data, size_t Size)
{
    if(p_Data == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Config.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    if(xSemaphoreTake(p_Config.Lock, 10 / portTICK_PERIOD_MS) == pdTRUE)
    {
        uart_write_bytes(p_Config.Interface, p_Data, Size);
        xSemaphoreGive(p_Config.Lock);

        return SIM70XX_ERR_OK;
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM70XX_UART_SendLine(SIM70XX_UART_Conf_t& p_Config, std::string Data)
{
    if(p_Config.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    if(xSemaphoreTake(p_Config.Lock, 10 / portTICK_PERIOD_MS) == pdTRUE)
    {
        uart_write_bytes(p_Config.Interface, Data.c_str(), Data.size());
        uart_write_bytes(p_Config.Interface, "\r\n", 2);
        xSemaphoreGive(p_Config.Lock);

        return SIM70XX_ERR_OK;
    }

    return SIM70XX_ERR_FAIL;
}

int SIM70XX_UART_Read(SIM70XX_UART_Conf_t& p_Config)
{
    size_t Avail;

    Avail = SIM70XX_UART_Available(p_Config);
    if(Avail != 0)
    {
        return SIM7020_UART_Get(p_Config);
    }

    return -1;
}

std::string SIM70XX_UART_ReadStringUntil(SIM70XX_UART_Conf_t& p_Config, char Terminator, uint32_t Timeout)
{
    int c;
    std::string Return;

    if(p_Config.isInitialized == false)
    {
        return std::string();
    }

    c = SIM7020_UART_TimedRead(p_Config, Timeout);
    while((c >= 0) && (c != Terminator))
    {
        Return += (char)c;
        c = SIM7020_UART_TimedRead(p_Config, Timeout);
    }

    return Return;
}

std::string SIM70XX_UART_ReadString(SIM70XX_UART_Conf_t& p_Config, uint32_t Timeout)
{
    int c;
    std::string Return;

    if(p_Config.isInitialized == false)
    {
        return std::string();
    }

    c = SIM7020_UART_TimedRead(p_Config, Timeout);
    while(c >= 0)
    {
        Return += (char)c;
        c = SIM7020_UART_TimedRead(p_Config, Timeout);
    }

    return Return;
}

void SIM70XX_UART_Flush(SIM70XX_UART_Conf_t& p_Config)
{
    if(p_Config.isInitialized == false)
    {
        return;
    }

    if(xSemaphoreTake(p_Config.Lock, 10 / portTICK_PERIOD_MS) == pdTRUE)
    {
        uart_flush(p_Config.Interface);
        xSemaphoreGive(p_Config.Lock);
    }
}

size_t SIM70XX_UART_Available(SIM70XX_UART_Conf_t& p_Config)
{
    size_t Avail = 0;

    if((p_Config.Lock == NULL) || (p_Config.isInitialized == false))
    {
        return Avail;
    }

    if(xSemaphoreTake(p_Config.Lock, 10 / portTICK_PERIOD_MS) == pdTRUE)
    {
        uart_get_buffered_data_len(p_Config.Interface, &Avail);
        xSemaphoreGive(p_Config.Lock);
    }

    return Avail;
}