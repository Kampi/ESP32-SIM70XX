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
#include <esp_log.h>

#include <driver/uart.h>
#include <driver/gpio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#include "sim70xx_uart.h"
#include "sim70xx_tools.h"

#include <sdkconfig.h>

#ifndef CONFIG_SIM70XX_UART_BUFFER_SIZE
    #define CONFIG_SIM70XX_UART_BUFFER_SIZE                 256
#endif

static uart_config_t _UART_Config = {
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

static gpio_config_t _GPIO_Config = {
    .pin_bit_mask           = 0,
    .mode                   = GPIO_MODE_OUTPUT,
    .pull_up_en             = GPIO_PULLUP_DISABLE,
    .pull_down_en           = GPIO_PULLDOWN_DISABLE,
    .intr_type              = GPIO_INTR_DISABLE,
};

static const char* TAG = "SIM70XX_UART";

/** @brief          Get a character from the UART FIFO.
 *  @param p_Config Pointer to SIM70XX UART configuration object
 *  @return         Received character
 */
static uint8_t SIM7020_UART_Get(const SIM70XX_UART_Conf_t* p_Config)
{
    uint8_t c;

    if((p_Config == NULL) || (p_Config->Lock == NULL) || (p_Config->isInitialized == false))
    {
        return 0;
    }

    xSemaphoreTake(p_Config->Lock, portMAX_DELAY);
    if(uart_read_bytes(p_Config->Interface, &c, sizeof(uint8_t), 20 / portTICK_RATE_MS) == 0)
    {
        c = 0;
    }
    xSemaphoreGive(p_Config->Lock);

    return c;
}

/** @brief          Read a character from the serial interface.
 *  @param p_Config Pointer to SIM70XX UART configuration object
 *  @return         Received character or -1 when no bytes available
 */
static int SIM70XX_UART_Read(const SIM70XX_UART_Conf_t* p_Config)
{
    if(SIM70XX_UART_Available(p_Config))
    {
        return SIM7020_UART_Get(p_Config);
    }

    return -1;
}

/** @brief          Wait for a character and read it.
 *  @param p_Config Pointer to SIM70XX UART configuration object
 *  @param Timeout  (Optional) Read timeout
 *  @return         Received character or -1 when no character received
 */
static int SIM7020_UART_TimedRead(const SIM70XX_UART_Conf_t* p_Config, uint32_t Timeout = 1000)
{
    int c;
    unsigned long Now;

    Now = SIM70XX_Tools_GetmsTimer();
    do
    {
        c = SIM70XX_UART_Read(p_Config);
        if(c >= 0)
        {
            return c;
        }
    } while((SIM70XX_Tools_GetmsTimer() - Now) < Timeout);

    return -1;
}

SIM70XX_Error_t SIM70XX_UART_Init(SIM70XX_UART_Conf_t* p_Config)
{
    if(p_Config == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Config->isInitialized)
    {
        SIM70XX_ERROR_CHECK(SIM70XX_UART_Deinit(p_Config));
    }

    p_Config->isInitialized = false;

    _UART_Config.baud_rate = p_Config->Baudrate;

    if((uart_driver_install(p_Config->Interface, CONFIG_SIM70XX_UART_BUFFER_SIZE * 2, 0, 0, NULL, 0) ||
        uart_param_config(p_Config->Interface, &_UART_Config) ||
        uart_set_pin(p_Config->Interface, p_Config->Tx, p_Config->Rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE)) != ESP_OK)
    {
        return SIM70XX_ERR_INVALID_STATE;
    }

    p_Config->Lock = xSemaphoreCreateMutex();
    if(p_Config->Lock == NULL)
    {
        return SIM70XX_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "UART initialized...");

    p_Config->isInitialized = true;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM70XX_UART_Deinit(SIM70XX_UART_Conf_t* p_Config)
{
    if(p_Config == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Config->isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    if(uart_is_driver_installed(p_Config->Interface))
    {
        uart_flush(p_Config->Interface);
        uart_driver_delete(p_Config->Interface);
    }

    vSemaphoreDelete(p_Config->Lock);

    _GPIO_Config.pin_bit_mask = ((1ULL << p_Config->Rx) | (1ULL << p_Config->Tx));
    gpio_config(&_GPIO_Config);
    gpio_set_level(p_Config->Rx, true);
    gpio_set_level(p_Config->Tx, true);

    ESP_LOGI(TAG, "UART deinitialized...");

    p_Config->isInitialized = false;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM70XX_UART_SendCommand(const SIM70XX_UART_Conf_t* p_Config, std::string Command)
{
    if(p_Config == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Config->isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    uart_write_bytes(p_Config->Interface, (const char*)Command.c_str(), Command.length());
    uart_write_bytes(p_Config->Interface, "\r\n", 2);

    return SIM70XX_ERR_OK;
}

std::string SIM70XX_UART_ReadStringUntil(const SIM70XX_UART_Conf_t* p_Config, char Terminator)
{
    int c;
    std::string Return;

    if((p_Config == NULL) || (p_Config->isInitialized == false))
    {
        return std::string();
    }

    c = SIM7020_UART_TimedRead(p_Config);
    while((c >= 0) && (c != Terminator))
    {
        Return += (char)c;
        c = SIM7020_UART_TimedRead(p_Config);
    }

    return Return;
}

std::string SIM70XX_UART_ReadString(const SIM70XX_UART_Conf_t* p_Config)
{
    int c;
    std::string Return;

    if((p_Config == NULL) || (p_Config->isInitialized == false))
    {
        return std::string();
    }

    c = SIM7020_UART_TimedRead(p_Config);
    while(c >= 0)
    {
        Return += (char)c;
        c = SIM7020_UART_TimedRead(p_Config);
    }

    return Return;
}

void SIM70XX_UART_Flush(const SIM70XX_UART_Conf_t* p_Config)
{
    assert(p_Config);

    if((p_Config == NULL) || (p_Config->isInitialized == false))
    {
        return;
    }

    uart_flush(p_Config->Interface);
}

int SIM70XX_UART_Available(const SIM70XX_UART_Conf_t* p_Config)
{
    size_t Avail;

    if((p_Config == NULL) || (p_Config->Lock == NULL) || (p_Config->isInitialized == false))
    {
        return 0;
    }

    xSemaphoreTake(p_Config->Lock, portMAX_DELAY);
    uart_get_buffered_data_len(p_Config->Interface, &Avail);
    xSemaphoreGive(p_Config->Lock);

    return Avail;
}