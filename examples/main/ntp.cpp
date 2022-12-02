 /*
 * ntp.cpp
 * 
 *  Copyright (C) Daniel Kampert, 2022
 *	Website: www.kampis-elektroecke.de
 *  File info: SIM70XX NTP example.
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

#ifdef CONFIG_DEMO_USE_SNTP

#include <esp_log.h>

#include <string.h>

#include "example.h"

static const char* TAG                              = "NTP";

#if(CONFIG_SIMXX_DEV == 7020)
    #define SIM70XX_NTP_Sync(Device, Time)          SIM7020_NTP_Sync(Device, CONFIG_DEMO_SNTP_HOST, CONFIG_DEMO_SNTP_TIMEZONE, Time)
    #define SIM70XX_NTP_Get(Device, Time)           SIM7020_NTP_GetTime(Device, Time)
#elif(CONFIG_SIMXX_DEV == 7080)
    #define SIM70XX_NTP_Sync(Device, Time)          SIM7080_NTP_Sync(Device, CONFIG_DEMO_SNTP_HOST, CONFIG_DEMO_SNTP_TIMEZONE, Time)
    #define SIM70XX_NTP_Get(Device, Time)           SIM7080_NTP_GetTime(Device, Time)
#endif

void NTP_Run(DEVICE_TYPE& p_Device)
{
    struct tm NTP_Time;
    struct tm Local_Time;

	ESP_LOGI(TAG, "Run NTP example...");

    memset(&NTP_Time, 0, sizeof(NTP_Time));
    memset(&Local_Time, 0, sizeof(Local_Time));

    ESP_LOGI(TAG, "Sync time...");
    SIM70XX_NTP_Sync(p_Device, &NTP_Time);
    ESP_LOGI(TAG, "     Seconds after the minute: %u", NTP_Time.tm_sec);
    ESP_LOGI(TAG, "     Minutes after the hour: %u", NTP_Time.tm_min);
    ESP_LOGI(TAG, "     Hours since midnight: %u", NTP_Time.tm_hour);
    ESP_LOGI(TAG, "     Day of the month. %u", NTP_Time.tm_mday);
    ESP_LOGI(TAG, "     Months since January: %u", NTP_Time.tm_mon);
    ESP_LOGI(TAG, "     Years since 1900: %u", NTP_Time.tm_year);
    ESP_LOGI(TAG, "     Days since Sunday: %u", NTP_Time.tm_wday);
    ESP_LOGI(TAG, "     Days since January 1: %u", NTP_Time.tm_yday);
    ESP_LOGI(TAG, "     Daylight Saving Time flag: %i", NTP_Time.tm_isdst);

    ESP_LOGI(TAG, "Read local time...");
    SIM70XX_NTP_Get(p_Device, &Local_Time);
    ESP_LOGI(TAG, "     Seconds after the minute: %u", Local_Time.tm_sec);
    ESP_LOGI(TAG, "     Minutes after the hour: %u", Local_Time.tm_min);
    ESP_LOGI(TAG, "     Hours since midnight: %u", Local_Time.tm_hour);
    ESP_LOGI(TAG, "     Day of the month. %u", Local_Time.tm_mday);
    ESP_LOGI(TAG, "     Months since January: %u", Local_Time.tm_mon);
    ESP_LOGI(TAG, "     Years since 1900: %u", Local_Time.tm_year);
    ESP_LOGI(TAG, "     Days since Sunday: %u", Local_Time.tm_wday);
    ESP_LOGI(TAG, "     Days since January 1: %u", Local_Time.tm_yday);
    ESP_LOGI(TAG, "     Daylight Saving Time flag: %i", Local_Time.tm_isdst);
}

#endif