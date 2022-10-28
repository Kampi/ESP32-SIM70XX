 /*
 * examples.cpp
 * 
 *  Copyright (C) Daniel Kampert, 2022
 *	Website: www.kampis-elektroecke.de
 *  File info: SIM70XX example application.
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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include "example.h"

#if(CONFIG_SIMXX_DEV == 7020)
    static SIM7020_Config_t _Config                                 = SIM70XX_DEFAULT_CONF_1NCE(UART_NUM_1, SIM_BAUD_115200, GPIO_NUM_13, GPIO_NUM_14);
    static SIM7020_t _Device;
    static SIM7020_Info_t _DeviceInfo;

    #define SIMXX_Init(Device, Config)                              SIM7020_Init(Device, Config)
    #define SIMXX_GetDeviceInfo(Device, Info)                       SIM7020_Info_GetDeviceInformation(Device, Info);
    #define SIMXX_PrintDeviceInfo(Info)                             SIM7020_Info_Print(Info);
    #define SIMXX_GetAvailOperators(Device, List)                   SIM7020_GetAvailOperators(Device, List);
#elif(CONFIG_SIMXX_DEV == 7080)
    static SIM7080_Config_t _Config                                 = SIM70XX_DEFAULT_CONF_1NCE(UART_NUM_1, SIM_BAUD_115200, GPIO_NUM_13, GPIO_NUM_14);
    static SIM7080_t _Device;
    static SIM7080_Info_t _DeviceInfo;

    #define SIMXX_Init(Device, Config)                              SIM7080_Init(Device, Config)
    #define SIMXX_GetDeviceInfo(Device, Info)                       SIM7080_Info_GetDeviceInformation(Device, Info);
    #define SIMXX_PrintDeviceInfo(Info)                             SIM7080_Info_Print(Info);
    #define SIMXX_GetAvailOperators(Device, List)                   SIM7080_GetAvailOperators(Device, List);
#endif

static const char* TAG = "SIM70XX_Example";

void StartExamples(void)
{
    if(SIM70XX_Tools_EnableModule(_Config.UART) == false)
    {
        ESP_LOGE(TAG, "Can not enable module!");
		return;
    }

    #if(CONFIG_SIMXX_DEV == 7080)
		// Add the frequency bands for the SIM7080 module.
        _Config.Bandlist.push_back(5);
        _Config.Bandlist.push_back(8);
        _Config.Bandlist.push_back(18);
        _Config.Bandlist.push_back(19);
        _Config.Bandlist.push_back(26);
    #endif

    if(SIMXX_Init(_Device, _Config) == SIM70XX_ERR_OK)
    {
        std::vector<SIM70XX_Operator_t> Operators;

        SIMXX_GetDeviceInfo(_Device, &_DeviceInfo);
        SIMXX_PrintDeviceInfo(&_DeviceInfo);

        #if(CONFIG_SIMXX_DEV == 7020)
            SIM7020_NetState_t NetworkStatus;
            std::vector<SIM7020_PDP_Status_t> Contextes;

            SIM7020_Info_GetNetworkStatus(_Device, &NetworkStatus);
        #elif(CONFIG_SIMXX_DEV == 7080)
            SIM7080_UEInfo_t UEInfo;
            std::vector<SIM7080_PDP_Network_t> Networks;

            SIM7080_IP_AutoAPN(_Device, _Config.APN);
            SIM7080_PDP_IP_CheckNetworks(_Device, &Networks);
            SIM7080_Info_GetEquipmentInfo(_Device, &UEInfo);
        #endif

        ESP_LOGI(TAG, "Run the examples...");

        #ifdef CONFIG_DEMO_USE_FS
            FileSystem_Run(_Device);
        #endif

        #ifdef CONFIG_DEMO_USE_SNTP
            NTP_Run(_Device);
        #endif

        #ifdef CONFIG_DEMO_USE_MQTT
            MQTT_Run(_Device);
        #endif

        ESP_LOGI(TAG, "Done...");
    }
}