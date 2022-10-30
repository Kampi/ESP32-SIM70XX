 /*
 * email.cpp
 * 
 *  Copyright (C) Daniel Kampert, 2022
 *	Website: www.kampis-elektroecke.de
 *  File info: SIM70XX E-Mail example.
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

#ifdef CONFIG_DEMO_USE_EMAIL

#include <esp_log.h>

#include "example.h"

#if(CONFIG_SIMXX_DEV == 7080)
    static SIM7080_EMail_Config_t _EMail_Config = {
        .Timeout        = 30,
        .SMTP_Address   = CONFIG_DEMO_SNTP_SERVER,
        .SMTP_Port      = CONFIG_DEMO_SNTP_PORT,
        .SMTP_Auth_User = CONFIG_DEMO_SNTP_USER,
        .SMTP_Auth_Pass = CONFIG_DEMO_SNTP_PASSWORD,
        .isUTF8         = false,
    };

    SIM7080_EMail_User_t Sender = {
        .Address        = CONFIG_DEMO_SNTP_USER,
        .Name           = "Kampi"
    };

    SIM7080_EMail_User_t To = {
        .Address        = CONFIG_DEMO_SNTP_USER,
        .Name           = "Kampi"
    };

    #define SIM70XX_SSL_Enable(Device, Enable)                              SIM7080_EMail_EnableSSL(Device, Enable)
    #define SIM70XX_Mail_SendText(Device, Config, From, To, Subject, Body)  SIM7080_EMail_SendText(Device, Config, From, To, Subject, Body)
#endif

static const char* TAG = "E-Mail";

void EMail_Run(DEVICE_TYPE& p_Device)
{
    SIM70XX_Error_t Error;

    #ifdef CONFIG_DEMO_USE_SSL
        Error = SIM70XX_SSL_Enable(p_Device, true);
        if(Error != SIM70XX_ERR_OK)
        {
            ESP_LOGE(TAG, "Can not enable SSL! Error: 0x%x", Error);
            return;
        }
    #endif

    ESP_LOGI(TAG, "Sending E-Mail...");
    Error = SIM70XX_Mail_SendText(p_Device, &_EMail_Config, &Sender, &To, "SIM70XX Test Mail", "Hello, World from Cellular!");
    if(Error != SIM70XX_ERR_OK)
    {
        ESP_LOGE(TAG, "Can not send E-Mail! Error: 0x%x", Error);
        return;
    }

    #ifdef CONFIG_DEMO_USE_POP3
        uint32_t Num;
        uint32_t Size;

        _EMail_Config.SMTP_Address = CONFIG_DEMO_POP3_SERVER;
        _EMail_Config.SMTP_Port = CONFIG_DEMO_POP3_PORT;

        Error = SIM7080_EMail_POP3_Login(p_Device, &_EMail_Config);
        if(Error != SIM70XX_ERR_OK)
        {
            ESP_LOGE(TAG, "Can not login to POP3 server! Error: 0x%x", Error);
            return;
        }

        ESP_LOGI(TAG, "Reading inbox...");
        Error = SIM7080_EMail_POP3_ReadInbox(p_Device, &Num, &Size);
        if(Error != SIM70XX_ERR_OK)
        {
            ESP_LOGE(TAG, "Can not get read inbox! Error: 0x%x", Error);
            return;
        }
        ESP_LOGI(TAG, "     Number of E-Mails: %u", Num);
        ESP_LOGI(TAG, "     Size: %u bytes", Size);

        ESP_LOGI(TAG, "Get E-Mails...");
        for(uint32_t i = 1; i <= Num; i++)
        {
            std::string ID;
            std::string Mail;

            ESP_LOGI(TAG, "     E-Mail %u / %u", i, Num);
            Error = SIM7080_EMail_POP3_ReadEMailMeta(p_Device, i, &Size, &ID);
            if(Error != SIM70XX_ERR_OK)
            {
                ESP_LOGE(TAG, "Can not get E-Mail meta data! Error: 0x%x", Error);
                break;
            }

            ESP_LOGI(TAG, "     Size: %u kB", Size);
            ESP_LOGI(TAG, "     ID: %s", ID.c_str());

            Error = SIM7080_EMail_POP3_ReadEMail(p_Device, i, &Mail);
            if(Error != SIM70XX_ERR_OK)
            {
                ESP_LOGE(TAG, "Can not read E-Mail! Error: 0x%x", Error);
                break;
            }

            ESP_LOGI(TAG, "E-Mail: %s", Mail.c_str());
            ESP_LOGI(TAG, "Size: %u", Mail.size());

            Error = SIM7080_EMail_POP3_DeleteEMail(p_Device, i);
            if(Error != SIM70XX_ERR_OK)
            {
                ESP_LOGE(TAG, "Can not delete E-Mail! Error: 0x%x", Error);
                break;
            }

            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        Error = SIM7080_EMail_POP3_Logout(p_Device);
        if(Error != SIM70XX_ERR_OK)
        {
            ESP_LOGE(TAG, "Can not logout from POP3 server! Error: 0x%x", Error);
            return;
        }
    #endif
}

#endif