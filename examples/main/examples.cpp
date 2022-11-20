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

static const char* TAG                                              = "SIM70XX_Example";

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

void StartExamples(void)
{
    if(SIM70XX_Tools_EnableModule(_Config.UART) == false)
    {
        SIM70XX_LOGE(TAG, "Can not enable module!");

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
        #ifdef CONFIG_DEMO_READ_OPERATOR
            SIM70XX_Error_t Error;
            std::vector<SIM70XX_Operator_t> Operators;

            Error = SIMXX_GetAvailOperators(_Device, &Operators);
            if(Error == SIM70XX_ERR_OK)
            {
                if(Operators.size() > 0)
                {
                    for(uint8_t i = 0; i < Operators.size(); i++)
                    {
                        SIM70XX_LOGI(TAG, "Operator %u:", i + 1);
                        SIM70XX_LOGI(TAG, "     Status: %u", Operators.at(i).Stat);
                        SIM70XX_LOGI(TAG, "     Act: %u", Operators.at(i).Act);
                        SIM70XX_LOGI(TAG, "     Long: %s", Operators.at(i).Long.c_str());
                        SIM70XX_LOGI(TAG, "     Short: %s", Operators.at(i).Short.c_str());
                        SIM70XX_LOGI(TAG, "     Numeric: %s", Operators.at(i).Numeric.c_str());
                    }
                }
                else
                {
                    SIM70XX_LOGI(TAG, "No operators found!");
                }
            }
            else
            {
                SIM70XX_LOGE(TAG, "Can not read operators. Error: 0x%X", Error);
            }
        #endif

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
/*
        SIM7080_PSM_Init(_Device);
        SIM7080_PSM_Enable(_Device, SIM7080_TAU_BASE_10_HOUR, 7, SIM7080_TIME_BASE_2_SECONDS, 1);

        while(1)
        {
            bool isActive;

            isActive = SIM7080_PSM_isActive(_Device);
            SIM70XX_LOGI(TAG, "PSM active: %u", isActive);

            if(isActive)
            {
                SIM7080_PwrMgnt_WakeUp(_Device);
            }

            vTaskDelay(10000 / portTICK_PERIOD_MS);
        }
*/
        SIM70XX_LOGI(TAG, "Run the examples...");

        #ifdef CONFIG_DEMO_USE_GNSS
            GNSS_Run(_Device);
        #endif

        #ifdef CONFIG_DEMO_USE_SSL
            SSL_Configure(_Device);
        #endif

        #ifdef CONFIG_DEMO_USE_NVRAM
            NVRAM_Run(_Device);
        #endif

        #ifdef CONFIG_DEMO_USE_FS
            FileSystem_Run(_Device);
        #endif

        #ifdef CONFIG_DEMO_USE_SNTP
            NTP_Run(_Device);
        #endif

        #ifdef CONFIG_DEMO_USE_TCPIP_CLIENT
            TCP_Client_Run_JSON(_Device);
            TCP_Client_Run_FormData(_Device);
        #endif

        #ifdef CONFIG_DEMO_USE_UDP_CLIENT
            UDP_Client_Run(_Device);
        #endif

        #ifdef CONFIG_DEMO_USE_TCPIP_SERVER
            TCP_Server_Run(_Device);
        #endif

        #ifdef CONFIG_DEMO_USE_EMAIL
            EMail_Run(_Device);
        #endif

        #ifdef CONFIG_DEMO_USE_MQTT
            MQTT_Run(_Device);
        #endif

        #ifdef CONFIG_DEMO_USE_COAP
            CoAP_Run(_Device);
        #endif

        #ifdef CONFIG_DEMO_USE_HTTP
            HTTP_Run(_Device);
        #endif

        SIM70XX_LOGI(TAG, "Done...");

        while(true)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}