 /*
 * sim7020_evt_coap.cpp
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

#if((CONFIG_SIMXX_DEV == 7020) && (defined CONFIG_SIM70XX_DRIVER_WITH_COAP))

#include "sim7020.h"
#include "sim7020_evt.h"

#include "../../Core/Queue/sim70xx_queue.h"

#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7020_Evt_CoAP";

void SIM7020_Evt_on_CoAP(SIM7020_t* const p_Device, std::string* p_Message)
{
    uint8_t ID;
    size_t Index;
    std::string Message;

    SIM70XX_LOGI(TAG, "CoAP event!");

    Index = p_Message->find("+CCOAPNMI");
    Index = p_Message->find(",", Index);
    ID = static_cast<uint8_t>(SIM70XX_Tools_StringToUnsigned(p_Message->substr(Index - 1, 1)));

    for(std::vector<SIM7020_CoAP_Socket_t*>::iterator it = p_Device->CoAP.Sockets.begin(); it != p_Device->CoAP.Sockets.end(); ++it)
    {
        if((*it)->Internal.ID == ID)
        {
            SIM70XX_LOGI(TAG, "Data received for socket: %u", ID);

            (*it)->Internal.isDataReceived = true;
        }
    }
}

#endif