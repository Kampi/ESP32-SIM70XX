 /*
 * sim7080_evt_mqtt.cpp
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

#if((CONFIG_SIMXX_DEV == 7080) && (defined CONFIG_SIM70XX_DRIVER_WITH_MQTT))

#include "sim7080.h"
#include "sim7080_evt.h"

#include "../../Core/Queue/sim70xx_queue.h"
#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7080_Evt_MQTT";

void SIM7080_Evt_on_MQTT_Subscribe(SIM7080_t* const p_Device, std::string* p_Message)
{
    SIM7080_MQTT_Sub_Evt_t* Message;

    Message = new SIM7080_MQTT_Sub_Evt_t;

    p_Message->erase(0, p_Message->find("\""));
    SIM70XX_Tools_StringRemove(p_Message);

    Message->Topic = SIM70XX_Tools_SubstringSplitErase(p_Message, ",");
    Message->Payload = *p_Message;
    SIMXX_TOOLS_REMOVE_LINEEND(Message->Payload);

    SIM70XX_LOGD(TAG, "Topic: %s", Message->Topic.c_str());
    SIM70XX_LOGD(TAG, "Payload: %s", Message->Payload.c_str());

    if(p_Device->MQTT.Socket == NULL)
    {
        return;
    }

	xQueueSend(p_Device->MQTT.Socket->Internal.SubQueue, &Message, 0);
}

#endif