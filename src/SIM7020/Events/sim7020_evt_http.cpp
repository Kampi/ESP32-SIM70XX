 /*
 * SIM7020_Evt_HTTP.cpp
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

#if((CONFIG_SIMXX_DEV == 7020) && (defined CONFIG_SIM70XX_DRIVER_WITH_HTTP))

#include <esp_log.h>

#include "sim7020.h"
#include "sim7020_evt.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7020_Evt_HTTP";

void SIM7020_Evt_on_HTTP_Event(SIM7020_t* const p_Device, std::string* p_Message)
{
    int Index;
    uint8_t ID;
    SIM7020_HTTP_Error_t HTTP_Error;

    ESP_LOGI(TAG, "HTTP disconnect event!");

    // Get the socket ID and the error code.
    SIMXX_TOOLS_REMOVE_LINEEND((*p_Message));

    Index = p_Message->find(",");
    if(Index == std::string::npos)
    {
        return;
    }

    ID = std::stoi(p_Message->substr(Index - 1, 1));
    HTTP_Error = (SIM7020_HTTP_Error_t)std::stoi(p_Message->substr(Index + 1));

    // Iterate through the list of active sockets and close the socket with the given ID.
    for(std::vector<SIM7020_HTTP_Socket_t*>::iterator it = p_Device->HTTP.Sockets.begin(); it != p_Device->HTTP.Sockets.end(); ++it)
    {
        if((*it)->ID == ID)
        {
            (*it)->isConnected = false;

            ESP_LOGI(TAG, "Disconnect socket %u", ID);
            ESP_LOGI(TAG, "Error: %i", HTTP_Error);
        }
    }
}

#endif