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

#include "sim7020.h"
#include "sim7020_evt.h"

#include "../../Core/Queue/sim70xx_queue.h"

#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7020_Evt_TCP";

void SIM7020_Evt_on_TCP_Disconnect(SIM7020_t* const p_Device, std::string* p_Message)
{
    uint8_t ID;
    size_t Index;
    SIM7020_TCP_Error_t TCP_Error;

    SIM70XX_LOGI(TAG, "TCP disconnect event!");

    Index = p_Message->find("+CSOERR");
    if(Index == std::string::npos)
    {
        return;
    }

    Index = p_Message->find(",", Index);
    ID = static_cast<uint8_t>(SIM70XX_Tools_StringToUnsigned(p_Message->substr(Index - 1, 1)));
    TCP_Error = (SIM7020_TCP_Error_t)std::stoi(p_Message->substr(Index + 1));

    // Iterate through the list of active sockets and close the socket with the given ID.
    for(std::vector<SIM7020_TCPIP_Socket_t*>::iterator it = p_Device->TCP.Sockets.begin(); it != p_Device->TCP.Sockets.end(); ++it)
    {
        if((*it)->Internal.ID == ID)
        {
            (*it)->Internal.isConnected = false;

            SIM70XX_LOGI(TAG, "Disconnect socket %u", ID);
            SIM70XX_LOGI(TAG, "Error: %i", TCP_Error);
        }
    }
}

void SIM7020_Evt_on_TCP_Data(SIM7020_t* const p_Device, std::string* p_Message)
{
    uint8_t ID;
    size_t Index;

    SIM70XX_LOGD(TAG, "TCP message data event!");

    Index = p_Message->find("+CSONMI");
    if(Index == std::string::npos)
    {
        return;
    }

    Index = p_Message->find(",", Index);
    ID = static_cast<uint8_t>(SIM70XX_Tools_StringToUnsigned(p_Message->substr(Index - 1, 1)));

    for(std::vector<SIM7020_TCPIP_Socket_t*>::iterator it = p_Device->TCP.Sockets.begin(); it != p_Device->TCP.Sockets.end(); ++it)
    {
        if((*it)->Internal.ID == ID)
        {
            SIM70XX_LOGI(TAG, "Data received for socket: %u", ID);

            (*it)->Internal.isDataReceived = true;
        }
    }
}

#endif