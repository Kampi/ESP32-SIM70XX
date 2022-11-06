 /*
 * coap.cpp
 * 
 *  Copyright (C) Daniel Kampert, 2022
 *	Website: www.kampis-elektroecke.de
 *  File info: SIM70XX CoAP example.
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

#ifdef CONFIG_DEMO_USE_COAP

#include <esp_log.h>

#include "example.h"

static std::string _CoAP_DNS_IP;

static const char* TAG                                              = "CoAP";

#if(CONFIG_SIMXX_DEV == 7020)
    static SIM7020_CoAP_Socket_t _CoAP_Socket;

    #define SIMXX_ParseDNS(Device, Host, IP)                        SIM7020_DNS_FetchAddress(Device, Host, IP)
    #define SIMXX_ClientCreate(Device, IP, Port, Socket)            SIM7020_CoAP_Create(Device, IP, Port, Socket)
    #define SIMXX_ClientTransmit(Device, Socket, Data, Length)      SIM7020_CoAP_Transmit(Device, Socket, Data, Length)
    #define SIMXX_ClientIsDataAvailable(Socket)                     SIM7020_CoAP_isDataAvailable(Socket)
    #define SIMXX_ClientReceive(Device, Socket, Response)           SIM7020_CoAP_Receive(Device, Socket, Response)
    #define SIMXX_ClientDestroy(Device, Socket)                     SIM7020_CoAP_Destroy(Device, Socket)
#elif(CONFIG_SIMXX_DEV == 7080)
    static SIM7080_CoAP_Socket_t _CoAP_Socket;

    #define SIMXX_ParseDNS(Device, Host, IP)                        SIM7080_DNS_FetchAddress(Device, Host, IP)
    #define SIMXX_ClientCreate(Device, IP, Port, Socket)            (SIM70XX_ERR_FAIL)
    #define SIMXX_ClientTransmit(Device, Socket, Data, Length)      (SIM70XX_ERR_FAIL)
    #define SIMXX_ClientIsDataAvailable(Socket)                     (false)
    #define SIMXX_ClientReceive(Device, Socket, Response)           (SIM70XX_ERR_FAIL)
    #define SIMXX_ClientDestroy(Device, Socket)                     (SIM70XX_ERR_FAIL)
#endif

void CoAP_Run(DEVICE_TYPE& p_Device)
{
    SIM70XX_Error_t Error;
    std::string Response;
    uint8_t Payload[] = {0x40, 0x01, 0x41, 0xC7, 0xB7, 0x63, 0x6F, 0x75, 0x6E, 0x74, 0x65, 0x72};

	ESP_LOGI(TAG, "Run CoAP example...");

    Error = SIMXX_ParseDNS(p_Device, CONFIG_DEMO_COAP_SERVER, &_CoAP_DNS_IP);
    if(Error != SIM70XX_ERR_OK)
    {
        ESP_LOGE(TAG, "Can not fetch IP address! Error: 0x%X", Error);
        return;
    }

    Error = SIMXX_ClientCreate(p_Device, _CoAP_DNS_IP, CONFIG_DEMO_COAP_PORT, &_CoAP_Socket);
    if(Error == SIM70XX_ERR_OK)
    {
        ESP_LOGI(TAG, "Connected with CoAP server: %s", _CoAP_DNS_IP.c_str());

        ESP_LOGI(TAG, "Transmit data...");
        Error = SIMXX_ClientTransmit(p_Device, &_CoAP_Socket, Payload, sizeof(Payload));
        if(Error == SIM70XX_ERR_OK)
        {
            while(SIMXX_ClientIsDataAvailable(&_CoAP_Socket) == false)
            {
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }

            SIMXX_ClientReceive(p_Device, &_CoAP_Socket, &Response);
            ESP_LOGI(TAG, "Response from server: %s", Response.c_str());
        }
        else
        {
            ESP_LOGE(TAG, "Can not transmit data! Error: 0x%X", Error);
        }

        SIMXX_ClientDestroy(p_Device, &_CoAP_Socket);
    }
    else
    {
        ESP_LOGE(TAG, "Can not create socket! Error: 0x%X", Error);
    }
}

#endif