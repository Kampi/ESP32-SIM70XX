 /*
 * sim7080_evt_tcp.cpp
 *
 *  Copyright (C) Daniel Kampert, 2022
 *	Website: www.kampis-elektroecke.de
 *  File info: SIM70XX driver for ESP32.
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

#if((CONFIG_SIMXX_DEV == 7080) && (defined CONFIG_SIM70XX_DRIVER_WITH_TCPIP))

#include <esp_log.h>

#include "sim7080.h"
#include "sim7080_evt.h"
#include "../../Core/Queue/sim70xx_queue.h"

static const char* TAG = "SIM7080_Evt_TCP";

void SIM7080_Evt_on_TCP_Disconnect(SIM7080_t* const p_Device, std::string* p_Message)
{
    uint8_t CID;
    size_t Index;
    std::string Message;
    SIM7080_TCP_Error_t Error;

    ESP_LOGD(TAG, "TCP disconnect event!");

    Index = p_Message->find("+CASTATE");
    if(Index == std::string::npos)
    {
        return;
    }

    Message = p_Message->substr(Index, p_Message->find("\r\n\r\n", Index) - Index);
    Index = Message.find(",");
    CID = SIM70XX_Tools_StringToUnsigned(Message.substr((Index - 1), Index));
    Error = (SIM7080_TCP_Error_t)SIM70XX_Tools_StringToUnsigned(Message.substr(Index + 1));

    for(std::vector<SIM7080_TCPIP_Socket_t*>::iterator it = p_Device->TCP.Sockets.begin(); it != p_Device->TCP.Sockets.end(); ++it)
    {
        if((*it)->Internal.CID == CID)
        {
            (*it)->Internal.isConnected = false;

            ESP_LOGI(TAG, "Disconnect socket: %u", CID);
            ESP_LOGI(TAG, "Error: %i", Error);
        }
    }
}

void SIM7080_Evt_on_TCP_DataReady(SIM7080_t* const p_Device, std::string* p_Message)
{
    size_t Index;
    uint8_t CID;
    std::string Message;

    ESP_LOGD(TAG, "TCP message data ready event!");

    Index = p_Message->find("+CADATAIND");
    if(Index == std::string::npos)
    {
        return;
    }

    Message = p_Message->substr(Index, p_Message->find("\r\n\r\n", Index) - Index);
    Index = Message.find(":");
    CID = SIM70XX_Tools_StringToUnsigned(Message.substr(Index + 1, Message.find("\r\n\r\n", Index) - Index + 1));

    for(std::vector<SIM7080_TCPIP_Socket_t*>::iterator it = p_Device->TCP.Sockets.begin(); it != p_Device->TCP.Sockets.end(); ++it)
    {
        if((*it)->Internal.CID == CID)
        {
            ESP_LOGI(TAG, "Data received for socket: %u", CID);

            (*it)->Internal.isDataReceived = true;
        }
    }
}

void SIM7080_Evt_on_TCP_Data(SIM7080_t* const p_Device, std::string* p_Message)
{
    size_t Index;
    std::string Message;
    std::string* Response;

    ESP_LOGD(TAG, "TCP message data event!");

    Index = p_Message->find("+CARECV");
    if(Index == std::string::npos)
    {
        return;
    }

    // Create a new response object, because we want to place the response in the event loop.
    Response = new std::string();
    *Response = p_Message->substr(Index, p_Message->find("\r\n\r\n", Index) - Index);

	if(xQueueSend(p_Device->Internal.EventQueue, &Response, 0) != pdPASS)
    {
        delete Response;
    }
}

#endif