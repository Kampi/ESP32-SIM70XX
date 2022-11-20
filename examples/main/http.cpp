 /*
 * http.cpp
 * 
 *  Copyright (C) Daniel Kampert, 2022
 *	Website: www.kampis-elektroecke.de
 *  File info: SIM70XX HTTP(S) example.
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

#ifdef CONFIG_DEMO_USE_HTTP

#include <esp_log.h>

#include <cJSON.h>
#include <string.h>

#include "example.h"

static const char* TAG                                              = "HTTP";

#if(CONFIG_SIMXX_DEV == 7020)
    static SIM7020_HTTP_Socket_t _HTTP_Socket;
    static SIM7020_HTTP_Response_t _HTTP_Response;

    #define SIMXX_Create(Device, Host, Socket)                                                  SIM7020_HTTP_Create(Device, Host, Socket)
    #define SIMXX_Connect(Device, Socket)                                                       SIM7020_HTTP_Connect(Device, Socket)
    #define SIMXX_GetRequest(Device, Socket, Path)                                              SIM7020_HTTP_GET(Device, Socket, Path)
    #define SIMXX_GetResponseItems(Socket)                                                      SIM7020_HTTP_GetResponseItems(Socket)
    #define SIMXX_GetResponse(Device, Socket, Response)                                         SIM7020_HTTP_GetResponse(Device, Socket, Response)
    #define SIMXX_PostRequest(Device, Socket, Path, ContentType, Header, Payload, ResponseCode) SIM7020_HTTP_POST(Device, Socket, Path, ContentType, Header, Payload, ResponseCode)
    #define SIMXX_Disconnect(Device, Socket)                                                    SIM7020_HTTP_Disconnect(Device, Socket)
    #define SIMXX_Destroy(Device, Socket)                                                       SIM7020_HTTP_Destroy(Device, Socket)
#elif(CONFIG_SIMXX_DEV == 7080)
    static SIM7080_HTTP_Socket_t _HTTP_Socket;
    static SIM7080_HTTP_Response_t _HTTP_Response;

    #define SIMXX_Create(Device, Host, Socket)                                                  SIM7080_HTTP_Create(Device, Host, Socket)
    #define SIMXX_Connect(Device, Socket)                                                       SIM7080_HTTP_Connect(Device, Socket)
    #define SIMXX_GetRequest(Device, Socket, Path, Payload)                                     SIM7080_HTTP_GET(Device, Socket, Path, Payload)
    #define SIMXX_GetResponseItems(Socket)                                                      SIM7080_HTTP_GetResponseItems(Socket)
    #define SIMXX_GetResponse(Device, Socket, Response)                                         SIM7080_HTTP_GetResponse(Device, Socket, Response)
    #define SIMXX_PostRequest(Device, Socket, Path, ContentType, Header, Payload, ResponseCode) SIM7080_HTTP_POST(Device, Socket, Path, ContentType, Header, Payload, ResponseCode)
    #define SIMXX_Disconnect(Device, Socket)                                                    SIM7080_HTTP_Disconnect(Device, Socket)
    #define SIMXX_Destroy(Device, Socket)                                                       SIM7080_HTTP_Destroy(Device, Socket)
#endif

void HTTP_Run(DEVICE_TYPE& p_Device)
{
    SIM70XX_Error_t Error;

    Error = SIMXX_Create(p_Device, CONFIG_DEMO_HTTP_SERVER, &_HTTP_Socket);
    if(Error == SIM70XX_ERR_OK)
    {
        SIM70XX_LOGI(TAG, "Connecting with server...");
        Error = SIMXX_Connect(p_Device, &_HTTP_Socket);
        if(Error == SIM70XX_ERR_OK)
        {
            std::string Header;
            std::string Payload;
            std::string ContentType;

            SIM70XX_LOGI(TAG, "Sending GET request...");
            Error = SIMXX_GetRequest(p_Device, &_HTTP_Socket, CONFIG_DEMO_HTTP_ENDPOINT);
            if(Error == SIM70XX_ERR_OK)
            {
                while(SIMXX_GetResponseItems(&_HTTP_Socket) == 0)
                {
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }

                SIM70XX_LOGI(TAG, "Response received!");

                Error = SIMXX_GetResponse(p_Device, &_HTTP_Socket, &_HTTP_Response);
                if(Error == SIM70XX_ERR_OK)
                {
                    SIM70XX_LOGI(TAG, " Response code: %u", _HTTP_Response.ResponseCode);
                    SIM70XX_LOGI(TAG, " Header: %s", _HTTP_Response.Header.c_str());
                    SIM70XX_LOGI(TAG, " Content: %s", _HTTP_Response.Content.c_str());
                    SIM70XX_LOGI(TAG, " Content length: %u", _HTTP_Response.Content.size());
                }
                else
                {
                    SIM70XX_LOGE(TAG, "Can not get response from queue! Error: 0x%X", Error);
                }
            }
            else
            {
                SIM70XX_LOGE(TAG, "Can not send GET request! Error: 0x%X", Error);
            }

            SIM70XX_LOGI(TAG, "Sending POST request...");
            Header = "";
            Payload = "{}";
            ContentType = "application/json";
            Error = SIMXX_PostRequest(p_Device, &_HTTP_Socket, CONFIG_DEMO_HTTP_ENDPOINT, ContentType, Header, Payload, &_HTTP_Response);
            if(Error == SIM70XX_ERR_OK)
            {
                SIM70XX_LOGI(TAG, " Response code: %u", _HTTP_Response.ResponseCode);
            }
            else
            {
                SIM70XX_LOGE(TAG, "Can not send POST request! Error: 0x%X", Error);
            }

            SIMXX_Disconnect(p_Device, &_HTTP_Socket);
        }
        else
        {
            SIM70XX_LOGE(TAG, "Can not connect with server! Error: 0x%X", Error);
        }

        SIMXX_Destroy(p_Device, &_HTTP_Socket);
    }
    else
    {
        SIM70XX_LOGE(TAG, "Can not create socket! Error: 0x%X", Error);
    }
}

#endif