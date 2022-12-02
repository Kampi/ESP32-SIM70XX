 /*
 * ssl.cpp
 * 
 *  Copyright (C) Daniel Kampert, 2022
 *	Website: www.kampis-elektroecke.de
 *  File info: SIM70XX SSL example.
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

#ifdef CONFIG_DEMO_USE_SSL

#include <esp_log.h>

#include "example.h"

extern const uint8_t Data_CAroot[]                                  asm("_binary_ISRG_Root_X1_pem_start");
extern const uint32_t Data_CAroot_Length                            asm("ISRG_Root_X1_pem_length");

static const char* TAG                                              = "SSL";

#if(CONFIG_SIMXX_DEV == 7080)
    static SIM7080_SSL_File_t _SSL_RootCA = {
        .Name = "ISRG_Root_X1.pem",
        .p_Data = Data_CAroot,
        .Size = (uint16_t)Data_CAroot_Length
    };

    static SIM7080_SSL_Config_t _SSL_Config = {
        .Version = SIM7080_SSL_VER_TLS_12,
        .CipherIndex = 0,
        .CiperSuite = SIM7080_SSL_RSA_WITH_AES_128_GCM_SHA256,
        .IgnoreTime = false
    };

    #define SIM70XX_SSL_Config(Device, Config)      SIM7080_SSL_Configure(Device, Config)
    #define SIM70XX_SSL_Enable(Device)              SIM7080_SSL_Enable(Device)
    #define SIM70XX_SSL_ImportRoot(Device)          SIM7080_SSL_ImportRoot(p_Device, SIM7080_FS_PATH_CUSTOMER, SIM7080_SSL_TYPE_TABLE, _SSL_RootCA)
#endif

void SSL_Configure(DEVICE_TYPE& p_Device)
{
    ESP_LOGI(TAG, "Configure SSL...");

    SIM70XX_SSL_Config(p_Device, &_SSL_Config);
    SIM70XX_SSL_Enable(p_Device);
    SIM70XX_SSL_ImportRoot(p_Device);
}

#endif