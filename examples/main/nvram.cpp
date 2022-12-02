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

#ifdef CONFIG_DEMO_USE_NVRAM

#include <esp_log.h>

#include "example.h"

static const char* TAG                             					= "NVRAM";

#if(CONFIG_SIMXX_DEV == 7020)
    #define SIMXX_Write(Device, Key, Data)              			SIM7020_NVRAM_Write(Device, Key, Data)
    #define SIMXX_Read(Device, Key, Data)                   		SIM7020_NVRAM_Read(Device, Key, Data)
    #define SIMXX_ReadKeys(Device, Keys)              				SIM7020_NVRAM_GetKeys(Device, Keys)
    #define SIM7020_NVRAM_Erase(Device, Key)                      	SIM7020_NVRAM_Erase(Device, Key)
#endif

void NVRAM_Run(DEVICE_TYPE& p_Device, std::string Key)
{
	std::string Data;
	SIM70XX_Error_t Error;
	std::vector<std::string> Keys;

	ESP_LOGI(TAG, "Run NVRAM example...");

	ESP_LOGI(TAG, "Writing data to NVRAM...");
	Error = SIMXX_Write(p_Device, Key, "Hello, World!");
	if(Error != SIM70XX_ERR_OK)
	{
		ESP_LOGE(TAG, "Can not write to NVRAM. Error: 0x%X", Error);
		return;
	}

	ESP_LOGI(TAG, "Reading data from NVRAM...");
	Error = SIMXX_Read(p_Device, Key, &Data);
	if(Error != SIM70XX_ERR_OK)
	{
		ESP_LOGE(TAG, "Can not read from NVRAM. Error: 0x%X", Error);
		return;
	}
	ESP_LOGI(TAG, "		Data: %s", Data.c_str());

	ESP_LOGI(TAG, "Reading keys from NVRAM...");
	Error = SIMXX_ReadKeys(p_Device, &Keys);
	if(Error != SIM70XX_ERR_OK)
	{
		ESP_LOGE(TAG, "Can not read keys from NVRAM. Error: 0x%X", Error);
		return;
	}

	for(uint32_t i = 0; i < Keys.size(); i++)
	{
		ESP_LOGI(TAG, "	Key %u: %s", i + 1, Keys.at(i).c_str());
	}

	ESP_LOGI(TAG, "Erase key from NVRAM...");
	Error = SIM7020_NVRAM_Erase(p_Device, Key);
	if(Error != SIM70XX_ERR_OK)
	{
		ESP_LOGE(TAG, "Can not erase key \"%s\". Error: 0x%X", Key.c_str(), Error);
	}
}

#endif