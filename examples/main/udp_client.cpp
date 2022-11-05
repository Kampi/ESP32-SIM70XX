 /*
 * udp_client.cpp
 * 
 *  Copyright (C) Daniel Kampert, 2022
 *	Website: www.kampis-elektroecke.de
 *  File info: SIM70XX UDP-Client example.
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

#ifdef CONFIG_DEMO_USE_UDP_CLIENT

#include <esp_log.h>

#include "example.h"

static const char* TAG                                              = "UDP";

#if(CONFIG_SIMXX_DEV == 7020)
    static SIM7020_Ping_t _UDP_Ping                                 = SIM7020_DEFAULT_PING(CONFIG_DEMO_UDP_CLIENT_HOST);
    static std::vector<SIM7020_PingRes_t> _UDP_PingResult;
    static SIM7020_TCPIP_Socket_t _UDP_Socket;

    #define SIMXX_RunPing(Device, Ping, Result)                     SIM7020_TCPIP_Ping(Device, Ping, Result)
    #define SIMXX_ClientCreate(Device, IP, Port, Socket)            SIM7020_UDP_Client_Create(Device, IP, Port, Socket)
    #define SIMXX_ClientConnect(Device, Socket)                     SIM7020_UDP_Client_Connect(Device, Socket)
    #define SIMXX_ClientTransmit(Device, Socket, Data, Length)      SIM7020_UDP_Client_Transmit(Device, Socket, Data, Length)
    #define SIMXX_ClientIsDataAvailable(Socket)                     SIM7020_UDP_Client_isDataAvailable(Socket)
    #define SIMXX_ClientReceive(Device, Socket, Response)           SIM7020_UDP_Client_Receive(Device, Socket, Response)
    #define SIMXX_ClientDisconnect(Device, Socket)                  SIM7020_UDP_Client_Disconnect(Device, Socket)
    #define SIMXX_ClientDestroy(Device, Socket)                     SIM7020_UDP_Client_Destroy(Device, Socket)
#elif(CONFIG_SIMXX_DEV == 7080)
    static SIM7080_Ping_t _UDP_Ping                                 = SIM7080_DEFAULT_PING(CONFIG_DEMO_UDP_CLIENT_HOST);
    static std::vector<SIM7080_PingRes_t> _UDP_PingResult;
    static SIM7080_TCPIP_Socket_t _UDP_Socket;

    #define SIMXX_RunPing(Device, Ping, Result)                     SIM7080_TCPIP_Ping(Device, Ping, Result)
    #define SIMXX_ClientCreate(Device, IP, Port, Socket)            SIM7080_UDP_Client_Create(Device, IP, Port, Socket)
    #define SIMXX_ClientConnect(Device, Socket)                     SIM7080_UDP_Client_Connect(Device, Socket)
    #define SIMXX_ClientTransmit(Device, Socket, Data, Length)      SIM7080_UDP_Client_Transmit(Device, Socket, Data, Length)
    #define SIMXX_ClientIsDataAvailable(Socket)                     SIM7080_UDP_Client_isDataAvailable(Socket)
    #define SIMXX_ClientReceive(Device, Socket, Response)           SIM7080_UDP_Client_Receive(Device, Socket, Response)
    #define SIMXX_ClientDisconnect(Device, Socket)                  SIM7080_UDP_Client_Disconnect(Device, Socket)
    #define SIMXX_ClientDestroy(Device, Socket)                     SIM7080_UDP_Client_Destroy(Device, Socket)
#endif

void UDP_Client_Run(DEVICE_TYPE& p_Device)
{
    SIM70XX_Error_t Error;

	ESP_LOGI(TAG, "Run UDP-Client example...");

    SIMXX_RunPing(p_Device, &_UDP_Ping, &_UDP_PingResult);

    for(uint8_t i = 0; i < _UDP_PingResult.size(); i++)
    {
        ESP_LOGI(TAG, "Result %u...", i + 1);
        ESP_LOGI(TAG, "     IP: %s", _UDP_PingResult.at(i).IP.c_str());
        ESP_LOGI(TAG, "     ReplyTime: %u", _UDP_PingResult.at(i).ReplyTime);
    }

    ESP_LOGI(TAG, "Connecting with UDP server: %s", std::string(CONFIG_DEMO_UDP_CLIENT_HOST).c_str());

    SIMXX_ClientCreate(p_Device, CONFIG_DEMO_UDP_CLIENT_HOST, CONFIG_DEMO_UDP_CLIENT_PORT, &_UDP_Socket);
    Error = SIMXX_ClientConnect(p_Device, &_UDP_Socket);
    if(Error == SIM70XX_ERR_OK)
    {
        std::string Payload = "Hello, World!";

        Error = SIMXX_ClientTransmit(p_Device, &_UDP_Socket, Payload.c_str(), Payload.size());
        if(Error != SIM70XX_ERR_OK)
        {
            ESP_LOGE(TAG, "Can not transmit payload! Error: 0x%X", Error);
        }

        while(SIMXX_ClientIsDataAvailable(&_UDP_Socket) == false)
        {
            ESP_LOGI(TAG, "Waiting for data...");

            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        SIMXX_ClientReceive(p_Device, &_UDP_Socket, &Payload);
        ESP_LOGI(TAG, "Response from server: %s", Payload.c_str());

        SIMXX_ClientDisconnect(p_Device, &_UDP_Socket);
        SIMXX_ClientDestroy(p_Device, &_UDP_Socket);
    }
    else
    {
        ESP_LOGE(TAG, "Can not connect with server! Error: 0x%X", Error);
    }
}

#endif