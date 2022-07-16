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

#if((CONFIG_SIMXX_DEV == 7020) && (defined CONFIG_SIM70XX_PROT_WITH_MQTT))

#include <esp_log.h>

#include "sim7020.h"
#include "sim7020_evt.h"
#include "../../Queue/sim70xx_queue.h"
#include "../../Commands/sim7020_commands.h"

static const char* TAG = "SIM7020_Evt_MQTT";

void SIM7020_Evt_MQTT_Pub(const SIM7020_t* const p_Device, std::string* p_Message)
{
    int Index;
    SIM7020_Pub_t* Packet;

    ESP_LOGI(TAG, "MQTT subscribe event!");

    Packet = new SIM7020_Pub_t();

    // Remove the command.
    p_Message->replace(p_Message->find("\n"), std::string("\n").size(), "");
    p_Message->replace(p_Message->find("\r"), std::string("\r").size(), "");
    p_Message->replace(p_Message->find("+CMQPUB: "), std::string("+CMQPUB: ").size(), "");

    // Get the socket ID.
    Index = p_Message->find(",");
    Packet->ID = std::stoi(p_Message->substr(0, Index));
    p_Message->erase(0, Index + 1);

    // Get the message topic.
    Index = p_Message->find(",");
    Packet->Topic = p_Message->substr(0, Index);
    p_Message->erase(0, Index + 1);

    // Get the quality of service.
    Index = p_Message->find(",");
    Packet->QoS = (SIM7020_MQTT_QoS_t)std::stoi(p_Message->substr(0, Index));
    p_Message->erase(0, Index + 1);

    // Get the retained flag.
    Index = p_Message->find(",");
    Packet->Retained = (bool)std::stoi(p_Message->substr(0, Index));
    p_Message->erase(0, Index + 1);

    // Get the duplicate flag.
    Index = p_Message->find(",");
    Packet->Dup = (bool)std::stoi(p_Message->substr(0, Index));
    p_Message->erase(0, Index + 1);

    // Skip the length.
    Index = p_Message->find(",");
    p_Message->substr(0, Index);
    p_Message->erase(0, Index + 1);

    // Get the message payload.
    Packet->Payload = p_Message->substr(0);

    Packet->Topic.replace(Packet->Topic.find("\""), std::string("\"").size(), "");
    Packet->Payload.replace(Packet->Payload.find("\""), std::string("\"").size(), "");

    if((p_Device->MQTT.SubQueue == NULL) || (xQueueSend(p_Device->MQTT.SubQueue, &Packet, 0) != pdPASS))
    {
        delete Packet;
    }

    delete p_Message;
}

void SIM7020_Evt_MQTT_Disconnect(SIM7020_t* const p_Device, std::string* p_Message)
{
    int Index;

    ESP_LOGI(TAG, "MQTT socket disconnect event!");

    p_Message->replace(p_Message->find("\n"), std::string("\n").size(), "");
    p_Message->replace(p_Message->find("\r"), std::string("\r").size(), "");

    Index = p_Message->find(":");

    // TODO: Event

    delete p_Message;
}

#endif