 /*
 * sim7020_evt_mqtt.cpp
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

#if((CONFIG_SIMXX_DEV == 7020) && (defined CONFIG_SIM70XX_DRIVER_WITH_MQTT))

#include "sim7020.h"
#include "sim7020_evt.h"

#include "../../Core/Queue/sim70xx_queue.h"

#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7020_Evt_MQTT";

void SIM7020_Evt_on_MQTT_Pub(SIM7020_t* const p_Device, std::string* p_Message)
{
    size_t Index;
    SIM7020_MQTT_Sub_Evt_t* SubMessage;

    SIM70XX_LOGI(TAG, "MQTT subscribe event!");

    SubMessage = new SIM7020_MQTT_Sub_Evt_t();

    SIMXX_TOOLS_REMOVE_LINEEND((*p_Message));

    p_Message->replace(p_Message->find("+CMQPUB: "), std::string("+CMQPUB: ").size(), "");

    // Get the socket ID.
    SubMessage->ID = SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(p_Message));

    // Get the message topic.
    SubMessage->Topic = SIM70XX_Tools_SubstringSplitErase(p_Message);

    // Get the quality of service.
    SubMessage->QoS = (SIM7020_MQTT_QoS_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(p_Message));

    // Get the retained flag.
    SubMessage->Retained = (bool)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(p_Message));

    // Get the duplicate flag.
    SubMessage->Dup = (bool)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(p_Message));

    SIM70XX_Tools_StringRemove(p_Message);

    // Skip the length.
    Index = p_Message->find(",");
    p_Message->substr(0, Index);
    p_Message->erase(0, Index + 1);

    // Get the message payload.
    SubMessage->Payload = p_Message->substr(0);

    // Add the message to the subscription queue.
    for(std::vector<SIM7020_MQTT_Socket_t*>::iterator it = p_Device->MQTT.Sockets.begin(); it != p_Device->MQTT.Sockets.end(); ++it)
    {
        if((*it)->Internal.ID == SubMessage->ID)
        {
            if((*it)->Internal.SubQueue == NULL)
            {
                xQueueSend((*it)->Internal.SubQueue, &SubMessage, 0);
            }
        }
    }
}

void SIM7020_Evt_on_MQTT_Disconnect(SIM7020_t* const p_Device, std::string* p_Message)
{
    uint8_t ID;
    size_t Index;

    SIM70XX_LOGI(TAG, "MQTT socket disconnect event!");

    Index = p_Message->find("+CMQDISCON");
    if(Index == std::string::npos)
    {
        return;
    }

    Index = p_Message->find("=", Index);
    SIMXX_TOOLS_REMOVE_LINEEND((*p_Message));

    ID = (uint8_t)SIM70XX_Tools_StringToUnsigned(p_Message->substr(Index - 1, 1));

    for(std::vector<SIM7020_MQTT_Socket_t*>::iterator it = p_Device->MQTT.Sockets.begin(); it != p_Device->MQTT.Sockets.end(); ++it)
    {
        if((*it)->Internal.ID == ID)
        {
            (*it)->Internal.isConnected = false;

            SIM70XX_LOGI(TAG, "Disconnect socket %u", ID);
        }
    }
}

#endif