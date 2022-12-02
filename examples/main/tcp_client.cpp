 /*
 * tcp_client.cpp
 * 
 *  Copyright (C) Daniel Kampert, 2022
 *	Website: www.kampis-elektroecke.de
 *  File info: SIM70XX TCP-Client example.
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

#ifdef CONFIG_DEMO_USE_TCPIP_CLIENT

#include <esp_log.h>

#include <cJSON.h>
#include <string.h>

#include "example.h"

extern const char Data_JSON[]                                       asm("_binary_Data_json_start");
extern const uint32_t Data_JSON_Length                              asm("Data_json_length");
extern const uint8_t Data_ImageSmall[]                              asm("_binary_ImageSmall_jpg_start");
extern const uint32_t Data_ImageSmall_Length                        asm("ImageSmall_jpg_length");
extern const uint8_t Data_ImageVeryLarge[]                          asm("_binary_ImageVeryLarge_jpg_start");
extern const uint32_t Data_ImageVeryLarge_Length                    asm("ImageVeryLarge_jpg_length");

static std::string _TCP_DNS_IP;

static const char* Boundary                                         = "data";

static const char* TAG                                              = "TCP-Client";

#if(CONFIG_SIMXX_DEV == 7020)
    static SIM7020_Ping_t _TCP_Ping                                 = SIM7020_DEFAULT_PING(CONFIG_DEMO_TCP_CLIENT_HOST);
    static std::vector<SIM7020_PingRes_t> _TCP_PingResult;
    static SIM7020_TCPIP_Socket_t _TCP_Socket;

    #define SIMXX_RunPing(Device, Ping, Result)                     SIM7020_TCPIP_Ping(Device, Ping, Result)
    #define SIMXX_ParseDNS(Device, Host, IP)                        SIM7020_DNS_FetchAddress(Device, Host, IP)
    #define SIMXX_ClientCreate(Device, IP, Port, Socket)            SIM7020_TCP_Client_Create(Device, IP, Port, Socket)
    #define SIMXX_ClientConnect(Device, Socket, Context)            SIM7020_TCP_Client_Connect(Device, Socket)
    #define SIMXX_ClientTransmit(Device, Socket, Data, Length)      SIM7020_TCP_Client_Transmit(Device, Socket, Data, Length)
    #define SIMXX_ClientIsDataAvailable(Socket)                     SIM7020_TCP_Client_isDataAvailable(Socket)
    #define SIMXX_ClientReceive(Device, Socket, Response)           SIM7020_TCP_Client_Receive(Device, Socket, Response)
    #define SIMXX_ClientDisconnect(Device, Socket)                  SIM7020_TCP_Client_Disconnect(Device, Socket)
    #define SIMXX_ClientDestroy(Device, Socket)                     SIM7020_TCP_Client_Destroy(Device, Socket)
#elif(CONFIG_SIMXX_DEV == 7080)
    static SIM7080_Ping_t _TCP_Ping                                 = SIM7080_DEFAULT_PING(CONFIG_DEMO_TCP_CLIENT_HOST);
    static std::vector<SIM7080_PingRes_t> _TCP_PingResult;
    static SIM7080_TCPIP_Socket_t _TCP_Socket;

    #define SIMXX_RunPing(Device, Ping, Result)                     SIM7080_TCPIP_Ping(Device, Ping, Result)
    #define SIMXX_ParseDNS(Device, Host, IP)                        SIM7080_DNS_FetchAddress(Device, Host, IP)
    #define SIMXX_ClientCreate(Device, IP, Port, Socket)            SIM7080_TCP_Client_Create(Device, IP, Port, Socket)
    #define SIMXX_ClientConnect(Device, Socket, Context)            SIM7080_TCP_Client_Connect(Device, Socket, (SIM7080_PDP_Context_t*)Context)
    #define SIMXX_ClientTransmit(Device, Socket, Data, Length)      SIM7080_TCP_Client_Transmit(Device, Socket, Data, Length)
    #define SIMXX_ClientIsDataAvailable(Socket)                     SIM7080_TCP_Client_isDataAvailable(Socket)
    #define SIMXX_ClientReceive(Device, Socket, Response)           SIM7080_TCP_Client_Receive(Device, Socket, Response)
    #define SIMXX_ClientDisconnect(Device, Socket)                  SIM7080_TCP_Client_Disconnect(Device, Socket)
    #define SIMXX_ClientDestroy(Device, Socket)                     SIM7080_TCP_Client_Destroy(Device, Socket)
#endif

/** @brief          Add a value to a POST request.
 *  @param p_Body   Pointer to HTTP message body
 *  @param Name     Name of the value
 *  @param Value    Value
 *  @param Boundary (Optional) Value boundary
 */
static void TCP_Client_AddHeader(std::string* p_Body, std::string Name, std::string Value, std::string Boundary = "boundary")
{
    p_Body->append("--" + Boundary + "\r\n");
    p_Body->append("Content-Disposition: form-data; name=" + Name + "\r\n");
    p_Body->append("\r\n");
    p_Body->append(Value);
    p_Body->append("\r\n");
}

void TCP_Client_Run_FormData(DEVICE_TYPE& p_Device, void* p_Opts)
{
    size_t Size;
    std::string Payload;
    std::string Header;
    std::string Footer;
    std::string Body;
    std::string HeaderImage;
    std::string Response;
    SIM70XX_Error_t Error;

	ESP_LOGI(TAG, "Run TCP-Client example...");

    Error = SIMXX_RunPing(p_Device, &_TCP_Ping, &_TCP_PingResult);
    if(Error != SIM70XX_ERR_OK)
    {
        ESP_LOGE(TAG, "Can not ping server! Error: 0x%X", Error);
        return;
    }

    Error = SIMXX_ParseDNS(p_Device, _TCP_Ping.Host, &_TCP_DNS_IP);
    if(Error != SIM70XX_ERR_OK)
    {
        ESP_LOGE(TAG, "Can not fetch IP address! Error: 0x%X", Error);
        return;
    }

    for(uint8_t i = 0; i < _TCP_PingResult.size(); i++)
    {
        ESP_LOGI(TAG, "Result %u...", i + 1);
        ESP_LOGI(TAG, "     IP: %s", _TCP_PingResult.at(i).IP.c_str());
        ESP_LOGI(TAG, "     ReplyTime: %u", _TCP_PingResult.at(i).ReplyTime);
    }

    ESP_LOGI(TAG, "Connecting with TCP server: %s", _TCP_DNS_IP.c_str());

    Footer = "--" + std::string(Boundary) + "--\r\n";

    HeaderImage += "--" + std::string(Boundary) + "\r\n";
    HeaderImage += "Content-Disposition: form-data; name=image; filename=" + std::string("ImageSmall.jpg") + "\r\n";
    HeaderImage += "\r\n";

    TCP_Client_AddHeader(&Body, "version", "1.0.0", std::string(Boundary));

    Size = Body.size() + Footer.size();
    Size += HeaderImage.size() + Data_ImageSmall_Length;

    Header += "POST " + std::string(CONFIG_DEMO_TCP_CLIENT_ENDPOINT) + " HTTP/1.1\r\n";
    Header += "Host: " + std::string(CONFIG_DEMO_TCP_CLIENT_HOST) + "\r\n";
    Header += "Content-Type: multipart/form-data; boundary=" + std::string(Boundary) + "\r\n";
    Header += "Content-Length: " + std::to_string(Size) + "\r\n";
    Header += "\r\n";

    SIMXX_ClientCreate(p_Device, _TCP_DNS_IP, CONFIG_DEMO_TCP_CLIENT_PORT, &_TCP_Socket);
    if(Error == SIM70XX_ERR_OK)
    {
        Error = SIMXX_ClientConnect(p_Device, &_TCP_Socket, p_Opts);
        if(Error == SIM70XX_ERR_OK)
        {
            // Transmit the HTTP header.
            SIMXX_ClientTransmit(p_Device, &_TCP_Socket, Header.c_str(), Header.size());

            // Transmit the form data.
            SIMXX_ClientTransmit(p_Device, &_TCP_Socket, Body.c_str(), Body.size());

            // Transmit the image data.
            SIMXX_ClientTransmit(p_Device, &_TCP_Socket, HeaderImage.c_str(), HeaderImage.size());
            SIMXX_ClientTransmit(p_Device, &_TCP_Socket, Data_ImageSmall, Data_ImageSmall_Length);
            SIMXX_ClientTransmit(p_Device, &_TCP_Socket, std::string("\r\n").c_str(), std::string("\r\n").size());

            // Transmit the message footer.
            SIMXX_ClientTransmit(p_Device, &_TCP_Socket, Footer.c_str(), Footer.size());

            while(SIMXX_ClientIsDataAvailable(&_TCP_Socket) == false)
            {
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }

            SIMXX_ClientReceive(p_Device, &_TCP_Socket, &Response);
            ESP_LOGI(TAG, "Response from server: %s", Response.c_str());

            SIMXX_ClientDisconnect(p_Device, &_TCP_Socket);
            SIMXX_ClientDestroy(p_Device, &_TCP_Socket);
        }
        else
        {
            ESP_LOGE(TAG, "Can not connect with server! Error: 0x%X", Error);
        }
    }
    else
    {
        ESP_LOGE(TAG, "Can not create socket! Error: 0x%X", Error);
    }
}

void TCP_Client_Run_JSON(DEVICE_TYPE& p_Device, void* p_Opts)
{
    std::string Header;
    std::string Response;
    SIM70XX_Error_t Error;

    cJSON* root;

	ESP_LOGI(TAG, "Run TCP-Client example...");

    Error = SIMXX_RunPing(p_Device, &_TCP_Ping, &_TCP_PingResult);
    if(Error != SIM70XX_ERR_OK)
    {
        ESP_LOGE(TAG, "Can not ping server! Error: 0x%X", Error);
        return;
    }

    Error = SIMXX_ParseDNS(p_Device, _TCP_Ping.Host, &_TCP_DNS_IP);
    if(Error != SIM70XX_ERR_OK)
    {
        ESP_LOGE(TAG, "Can not fetch IP address! Error: 0x%X", Error);
        return;
    }

    for(uint8_t i = 0; i < _TCP_PingResult.size(); i++)
    {
        ESP_LOGI(TAG, "Result %u...", i + 1);
        ESP_LOGI(TAG, "     IP: %s", _TCP_PingResult.at(i).IP.c_str());
        ESP_LOGI(TAG, "     ReplyTime: %u", _TCP_PingResult.at(i).ReplyTime);
    }

    ESP_LOGI(TAG, "Connecting with TCP server: %s", _TCP_DNS_IP.c_str());

    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "version", "1.0.0");

    ESP_LOGI(TAG, "Transmit JSON: %s", cJSON_Print(root));

    Header += "POST " + std::string(CONFIG_DEMO_TCP_CLIENT_ENDPOINT) + " HTTP/1.1\r\n";
    Header += "Host: " + std::string(CONFIG_DEMO_TCP_CLIENT_HOST) + "\r\n";
    Header += "Content-Type: application/json\r\n";
    Header += "Content-Length: " + std::to_string(strlen(cJSON_Print(root))) + "\r\n";
    Header += "\r\n";

    Error = SIMXX_ClientCreate(p_Device, _TCP_DNS_IP, CONFIG_DEMO_TCP_CLIENT_PORT, &_TCP_Socket);
    if(Error == SIM70XX_ERR_OK)
    {
        Error = SIMXX_ClientConnect(p_Device, &_TCP_Socket, p_Opts);
        if(Error == SIM70XX_ERR_OK)
        {
            // Transmit the HTTP header.
            SIMXX_ClientTransmit(p_Device, &_TCP_Socket, Header.c_str(), Header.size());

            // Transmit the form data.
            SIMXX_ClientTransmit(p_Device, &_TCP_Socket, cJSON_Print(root), strlen(cJSON_Print(root)));

            cJSON_Delete(root);

            while(SIMXX_ClientIsDataAvailable(&_TCP_Socket) == false)
            {
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }

            ESP_LOGI(TAG, "Data received...");
            SIMXX_ClientReceive(p_Device, &_TCP_Socket, &Response);
            ESP_LOGI(TAG, "Response from server: %s", Response.c_str());

            SIMXX_ClientDisconnect(p_Device, &_TCP_Socket);
            SIMXX_ClientDestroy(p_Device, &_TCP_Socket);
        }
        else
        {
            ESP_LOGE(TAG, "Can not connect with server! Error: 0x%X", Error);
        }
    }
    else
    {
        ESP_LOGE(TAG, "Can not create socket! Error: 0x%X", Error);
    }
}

#endif