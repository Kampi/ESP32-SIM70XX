 /*
 * sim7020_evt_tcp.cpp
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

#if((CONFIG_SIMXX_DEV == 7020) && (defined CONFIG_SIM70XX_DRIVER_WITH_TCPIP))

#include <esp_log.h>

#include "sim7020.h"
#include "sim7020_evt.h"
#include "../../Queue/sim70xx_queue.h"
#include "../../Commands/sim70xx_commands.h"

static const char* TAG = "SIM7020_Evt_TCP";

void SIM7020_Evt_TCP_Disconnect(SIM7020_t* const p_Device, std::string* p_Message)
{
    int Index;
    uint8_t ID;
    SIM7020_TCP_Error_t TCP_Error;

    ESP_LOGI(TAG, "TCP disconnect event!");

    // Get the socket ID and the error code.
    p_Message->replace(p_Message->find("\n"), std::string("\n").size(), "");
    p_Message->replace(p_Message->find("\r"), std::string("\r").size(), "");
    Index = p_Message->find(",");
    ID = std::stoi(p_Message->substr(Index - 1, Index));
    TCP_Error = (SIM7020_TCP_Error_t)std::stoi(p_Message->substr(Index));

    // Iterate through the list of active sockets and close the socket with the given ID.
    for(std::vector<SIM7020_TCP_Socket_t*>::iterator it = p_Device->TCP.Sockets.begin(); it != p_Device->TCP.Sockets.end(); ++it)
    {
        if((*it)->ID == ID)
        {
            (*it)->isConnected = false;

            ESP_LOGI(TAG, "Disconnect socket %u", ID);
            ESP_LOGI(TAG, "Error: %i", TCP_Error);
        }
    }

    delete p_Message;
}

#endif