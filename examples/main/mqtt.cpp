 /*
 * mqtt.cpp
 * 
 *  Copyright (C) Daniel Kampert, 2022
 *	Website: www.kampis-elektroecke.de
 *  File info: SIM70XX MQTT example.
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

#ifdef CONFIG_DEMO_USE_MQTT

#include <esp_log.h>

#include "example.h"

static const char* TAG                                              = "MQTT";

#if(CONFIG_SIMXX_DEV == 7020)
    static SIM7020_MQTT_Socket_t _MQTT_Socket;
    static SIM7020_MQTT_Sub_Evt_t _MQTT_Message;

    #define SIMXX_Create(Device, Socket)                            SIM7020_MQTT_Create(Device, Socket)
    #define SIMXX_Connect(Device, Socket)                           SIM7020_MQTT_Connect(Device, Socket)
    #define SIMXX_Subscribe(Device, Socket, Topic)                  SIM7020_MQTT_Subscribe(Device, Socket, Topic)
    #define SIMXX_WaitSubscription(Socket)                          SIM7020_MQTT_GetSubcriptionItems(Socket)
    #define SIMXX_GetSubscription(Device, Socket, Message)          SIM7020_MQTT_GetSubscription(Device, Socket, Message)
    #define SIMXX_Unsubscribe(Device, Socket, Topic)                SIM7020_MQTT_Unsubscribe(Device, Socket, Topic)
    #define SIMXX_Publish(Device, Socket, Topic, Payload)           SIM7020_MQTT_Publish(Device, Socket, Topic, Payload)
    #define SIMXX_Disconnect(Device, Socket)                        SIM7020_MQTT_Disconnect(Device, Socket)
    #define SIMXX_Destroy(Device, Socket)                           SIM7020_MQTT_Destroy(Device, Socket)
#elif(CONFIG_SIMXX_DEV == 7080)
    static SIM7080_MQTT_Socket_t _MQTT_Socket;
    static SIM7080_MQTT_Sub_Evt_t _MQTT_Message;

    #define SIMXX_Create(Device, Socket)                            SIM7080_MQTT_Create(Device, Socket)
    #define SIMXX_Connect(Device, Socket)                           SIM7080_MQTT_Connect(Device, Socket)
    #define SIMXX_Subscribe(Device, Socket, Topic)                  SIM7080_MQTT_Subscribe(Device, Socket, Topic)
    #define SIMXX_WaitSubscription(Socket)                          SIM7080_MQTT_GetSubcriptionItems(Socket)
    #define SIMXX_GetSubscription(Device, Socket, Message)          SIM7080_MQTT_GetSubscription(Device, Socket, Message)
    #define SIMXX_Unsubscribe(Device, Socket, Topic)                SIM7080_MQTT_Unsubscribe(Device, Socket, Topic)
    #define SIMXX_Publish(Device, Socket, Topic, Payload)           SIM7080_MQTT_Publish(Device, Socket, Topic, Payload)
    #define SIMXX_Disconnect(Device, Socket)                        SIM7080_MQTT_Disconnect(Device, Socket)
    #define SIMXX_Destroy(Device, Socket)                           SIM7080_MQTT_Destroy(Device, Socket)
#endif

void MQTT_Run(DEVICE_TYPE& p_Device, std::string SubTopic, std::string PubTopic)
{
    SIM70XX_Error_t Error;

	SIM70XX_LOGI(TAG, "Run MQTT example...");

    _MQTT_Socket.KeepAlive = 60;
    _MQTT_Socket.Broker = CONFIG_DEMO_MQTT_BROKER;
    _MQTT_Socket.Port = CONFIG_DEMO_MQTT_PORT;
    _MQTT_Socket.ClientID = "SIM70XX-MQTT";
    _MQTT_Socket.CleanSession = true;

    #ifdef CONFIG_DEMO_MQTT_USE_LOGIN
        _MQTT_Socket.Username = CONFIG_DEMO_MQTT_USER;
        _MQTT_Socket.Password = CONFIG_DEMO_MQTT_PASSWORD;
    #endif

    Error = SIMXX_Create(p_Device, &_MQTT_Socket);
    if(Error == SIM70XX_ERR_OK)
    {
        Error = SIMXX_Connect(p_Device, &_MQTT_Socket);
        if(Error == SIM70XX_ERR_OK)
        {
            SIMXX_Subscribe(p_Device, &_MQTT_Socket, SubTopic);
            SIMXX_Publish(p_Device, &_MQTT_Socket, PubTopic, "Hello from Cellular!");

            while(true)
            {
                while(SIMXX_WaitSubscription(&_MQTT_Socket) == 0):
                {
                    vTaskDelay(1000 / portTICK_PERIODE_MS);
                }

                SIM70XX_LOGI(TAG, "Receive subscription...");

                if(SIMXX_GetSubscription(&_MQTT_Socket, &_MQTT_Message) == SIM70XX_ERR_OK)
                {
                    SIM70XX_LOGI(TAG, "Topic: %s", _MQTT_Message.Topic.c_str());
                    SIM70XX_LOGI(TAG, "Payload: %s", _MQTT_Message.Payload.c_str());

                    break;
                }

                vTaskDelay(100 / portTICK_PERIOD_MS);
            }

            SIMXX_Unsubscribe(p_Device, &_MQTT_Socket, SubTopic);
            SIMXX_Disconnect(p_Device, &_MQTT_Socket);
        }
        else
        {
            SIM70XX_LOGE(TAG, "Can not connect socket! Error: 0x%X", Error);
        }

        SIMXX_Destroy(p_Device, &_MQTT_Socket);
    }
    else
    {
        SIM70XX_LOGE(TAG, "Can not create socket! Error: 0x%X", Error);
    }
}

#endif