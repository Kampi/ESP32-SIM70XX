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
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7080_Evt_TCP";

void SIM7080_Evt_on_TCP_Disconnect(SIM7080_t* const p_Device, std::string* p_Message)
{
    uint8_t ID;
    size_t Index;
    std::string Message;
    SIM7080_TCP_Error_t Error;

    ESP_LOGI(TAG, "TCP disconnect event!");

    SIMXX_TOOLS_REMOVE_LINEEND((*p_Message));

    Index = p_Message->find("+CASTATE:");
    Message = p_Message->substr(Index + std::string("+CASTATE:").size() + 1, p_Message->find("\r\n", Index) - Index);
    p_Message->erase(Index, p_Message->find("\r\n", Index) - Index);

    Index = Message.find(",");
    ID = std::stoi(Message.substr((Index - 1), Index));
    Error = (SIM7080_TCP_Error_t)std::stoi(Message.substr(Index + 1));

    for(std::vector<SIM7080_TCP_Socket_t*>::iterator it = p_Device->TCP.Sockets.begin(); it != p_Device->TCP.Sockets.end(); ++it)
    {
        if((*it)->ID == ID)
        {
            (*it)->isConnected = false;

            ESP_LOGI(TAG, "Disconnect socket: %u", ID);
            ESP_LOGI(TAG, "Error: %i", Error);
        }
    }
}

void SIM7080_Evt_on_TCP_DataReceived(SIM7080_t* const p_Device, std::string* p_Message)
{
    uint8_t ID;
    size_t Index;
    std::string Message;

    ESP_LOGI(TAG, "TCP message received event!");

    SIMXX_TOOLS_REMOVE_LINEEND((*p_Message));

    Index = p_Message->find("+CADATAIND:");
    Message = p_Message->substr(Index + std::string("+CADATAIND:").size() + 1, p_Message->find("\r\n", Index) - Index);
    p_Message->erase(Index, p_Message->find("\r\n", Index) - Index);

    Index = Message.find(":");
    ID = std::stoi(Message.substr(Index + 1, Message.find("\r\n", Index) - Index + 1));

    for(std::vector<SIM7080_TCP_Socket_t*>::iterator it = p_Device->TCP.Sockets.begin(); it != p_Device->TCP.Sockets.end(); ++it)
    {
        if((*it)->ID == ID)
        {
            (*it)->isDataReceived = true;

            ESP_LOGI(TAG, "Data received for socket: %u", ID);
        }
    }
}

#endif