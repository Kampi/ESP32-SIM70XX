 /*
 * sim70xx_gpio.cpp
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

#include <driver/gpio.h>

#include "sim70xx_gpio.h"

static gpio_config_t _SIM70XX_PwrKey_Config = {
    .pin_bit_mask = BIT(CONFIG_SIM70XX_GPIO_PWRKEY_PIN),
    .mode = GPIO_MODE_OUTPUT,
    #ifdef CIONFIG_SIM70XX_GPIO_PWRKEY_ENABLE_PULL
        #ifdef CONFIG_SIM70XX_GPIO_PWRKEY_INVERT
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_ENABLE,
        #else
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
        #endif
    #else
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    #endif
    .intr_type = GPIO_INTR_DISABLE,
};

#ifdef CONFIG_SIM70XX_GPIO_USE_STATUS
    static gpio_config_t _SIM70XX_Status_Config = {
    };
#endif

#ifdef CONFIG_SIM70XX_GPIO_USE_NETLIGHT
    static gpio_config_t _SIM70XX_Netlight_Config = {
    };
#endif

static const char* TAG = "SIM70XX_GPIO";

void SIM70XX_GPIO_Init(void)
{
    ESP_LOGI(TAG, "Initialize GPIO...");

    gpio_config(&_SIM70XX_PwrKey_Config);

    #ifdef CONFIG_SIM70XX_GPIO_USE_STATUS
        ESP_LOGI(TAG, "     Use status signal: [x]");
        gpio_config(&_SIM70XX_Status_Config);
    #else
        ESP_LOGI(TAG, "     Use status signal: [ ]");
    #endif

    #ifdef CONFIG_SIM70XX_GPIO_USE_NETLIGHT
        ESP_LOGI(TAG, "     Use netlight signal: [x]");
        gpio_config(&_SIM70XX_Netlight_Config);
    #else
        ESP_LOGI(TAG, "     Use netlight signal: [ ]");
    #endif

    #ifdef CONFIG_SIM70XX_GPIO_PWRKEY_INVERT
        gpio_set_level((gpio_num_t)CONFIG_SIM70XX_GPIO_PWRKEY_PIN, false);
    #else
        gpio_set_level((gpio_num_t)CONFIG_SIM70XX_GPIO_PWRKEY_PIN, true);
    #endif
}