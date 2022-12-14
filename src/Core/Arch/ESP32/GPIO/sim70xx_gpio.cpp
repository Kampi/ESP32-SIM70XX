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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include "sim70xx_gpio.h"

#include "../Logging/sim70xx_logging.h"

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
        .pin_bit_mask = BIT(CONFIG_SIM70XX_GPIO_STATUS_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
#endif

#ifdef CONFIG_SIM70XX_GPIO_USE_NETLIGHT
    static gpio_config_t _SIM70XX_Netlight_Config = {
        .pin_bit_mask = BIT(CONFIG_SIM70XX_GPIO_NETLIGHT_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
#endif

#ifdef CONFIG_SIM70XX_GPIO_USE_RESET
    static gpio_config_t _SIM70XX_Reset_Config = {
        .pin_bit_mask = BIT(SIM70XX_GPIO_RESET_PIN),
        .mode = GPIO_MODE_OUTPUT,
        #ifdef SIM70XX_GPIO_RESET_ENABLE_PULL
            #ifdef SIM70XX_GPIO_RESET_INVERT
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
#endif

static const char* TAG = "SIM70XX_GPIO";

void SIM70XX_GPIO_Init(void)
{
    SIM70XX_LOGI(TAG, "Initialize GPIO...");

    esp_log_level_set("gpio", ESP_LOG_NONE);

    assert(GPIO_IS_VALID_GPIO(CONFIG_SIM70XX_GPIO_PWRKEY_PIN));

    #ifdef CONFIG_SIM70XX_GPIO_USE_RESET
        assert(GPIO_IS_VALID_GPIO(CONFIG_SIM70XX_GPIO_RESET_PIN));
    #endif

    #ifdef CONFIG_SIM70XX_GPIO_USE_STATUS
        assert(GPIO_IS_VALID_GPIO(CONFIG_SIM70XX_GPIO_STATUS_PIN));
    #endif

    #ifdef CONFIG_SIM70XX_GPIO_USE_NETLIGHT
        assert(GPIO_IS_VALID_GPIO(CONFIG_SIM70XX_GPIO_NETLIGHT_PIN));
    #endif

    gpio_config(&_SIM70XX_PwrKey_Config);

    #ifdef CONFIG_SIM70XX_GPIO_USE_RESET
        SIM70XX_LOGI(TAG, "     Reset signal: [x]");
        gpio_config(&_SIM70XX_Reset_Config);
    #else
        SIM70XX_LOGI(TAG, "     Reset signal: [ ]");
    #endif

    #ifdef CONFIG_SIM70XX_GPIO_USE_STATUS
        SIM70XX_LOGI(TAG, "     Status signal: [x]");
        gpio_config(&_SIM70XX_Status_Config);
    #else
        SIM70XX_LOGI(TAG, "     Status signal: [ ]");
    #endif

    #ifdef CONFIG_SIM70XX_GPIO_USE_NETLIGHT
        SIM70XX_LOGI(TAG, "     Netlight signal: [x]");
        gpio_config(&_SIM70XX_Netlight_Config);
    #else
        SIM70XX_LOGI(TAG, "     Netlight signal: [ ]");
    #endif

    #ifdef CONFIG_SIM70XX_GPIO_PWRKEY_INVERT
        gpio_set_level((gpio_num_t)CONFIG_SIM70XX_GPIO_PWRKEY_PIN, false);
    #else
        gpio_set_level((gpio_num_t)CONFIG_SIM70XX_GPIO_PWRKEY_PIN, true);
    #endif

    #ifdef CONFIG_SIM70XX_GPIO_USE_RESET
        #ifdef CONFIG_SIM70XX_GPIO_RESET_INVERT
            gpio_set_level((gpio_num_t)CONFIG_SIM70XX_GPIO_RESET_PIN, true);
        #else
            gpio_set_level((gpio_num_t)CONFIG_SIM70XX_GPIO_RESET_PIN, false);
        #endif
    #endif
}

void SIM70XX_GPIO_Deinit(void)
{
    gpio_reset_pin((gpio_num_t)CONFIG_SIM70XX_GPIO_PWRKEY_PIN);

    #ifdef CONFIG_SIM70XX_GPIO_USE_RESET
        gpio_reset_pin((gpio_num_t)CONFIG_SIM70XX_GPIO_RESET_PIN);
    #endif

    #ifdef CONFIG_SIM70XX_GPIO_USE_STATUS
        gpio_reset_pin((gpio_num_t)CONFIG_SIM70XX_GPIO_STATUS_PIN);
    #endif

    #ifdef CONFIG_SIM70XX_GPIO_USE_NETLIGHT
        gpio_reset_pin((gpio_num_t)CONFIG_SIM70XX_GPIO_NETLIGHT_PIN);
    #endif
}

#ifdef CONFIG_SIM70XX_GPIO_USE_RESET
	void SIM70XX_GPIO_HardReset(void)
	{
        #ifdef SIM70XX_GPIO_RESET_INVERT
            gpio_set_level((gpio_num_t)CONFIG_SIM70XX_GPIO_RESET_PIN, true);
        #else
            gpio_set_level((gpio_num_t)CONFIG_SIM70XX_GPIO_RESET_PIN, false);
        #endif

        vTaskDelay(1000 / portTICK_PERIOD_MS);

        #ifdef SIM70XX_GPIO_RESET_INVERT
            gpio_set_level((gpio_num_t)CONFIG_SIM70XX_GPIO_RESET_PIN, false);
        #else
            gpio_set_level((gpio_num_t)CONFIG_SIM70XX_GPIO_RESET_PIN, true);
        #endif
	}
#endif