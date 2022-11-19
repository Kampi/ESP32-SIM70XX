#include <stdio.h>

#include "example.h"

#ifdef CONFIG_PM_ENABLE
    #include <esp_pm.h>

    esp_pm_config_esp32_t Config = 
    {
        .max_freq_mhz = CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ,
        .min_freq_mhz = 80,
        .light_sleep_enable = true,
    };
#endif

extern "C" void app_main(void)
{
    #ifdef CONFIG_PM_ENABLE
        esp_pm_configure(&Config);
    #endif

    StartExamples();
}