 /*
 * sim7020_evt.cpp
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

#if(CONFIG_SIMXX_DEV == 7020)

#include "sim7020.h"
#include "sim7020_evt.h"

#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7020_Evt";

void SIM70XX_Evt_MessageFilter(void* p_Device, std::string* p_Message)
{
	// This flag is set to #true when the data were processed completely in the event handler. Do not set this flag to #true when
	// the data should be processed by using the event message queue.
	bool Processed;
	SIM7020_t* Device = (SIM7020_t*)p_Device;

	Processed = false;

	// Shutdown message was received.
	if(p_Message->find("NORMAL POWER DOWN") != std::string::npos)
	{
		SIM70XX_LOGI(TAG, "Power down event!");

		Device->Internal.isInitialized = false;
		Processed = true;
	}

	if(p_Message->find("EXIT PSM") != std::string::npos)
	{
		SIM7020_Evt_on_PSM_Event(Device, p_Message, false);
	}

	if(p_Message->find("ENTER PSM") != std::string::npos)
	{
		SIM7020_Evt_on_PSM_Event(Device, p_Message, true);
		Processed = true;
	}

	#ifdef CONFIG_SIM70XX_DRIVER_WITH_COAP
		if(p_Message->find("+CCOAPNMI") != std::string::npos)
		{
			SIM7020_Evt_on_CoAP(Device, p_Message);
		}
	#endif

	#ifdef CONFIG_SIM70XX_DRIVER_WITH_TCPIP
		if(p_Message->find("+CSOERR") != std::string::npos)
		{
			SIM7020_Evt_on_TCP_Disconnect(Device, p_Message);
			Processed = true;
		}

		if(p_Message->find("+CSONMI") != std::string::npos)
		{
			SIM7020_Evt_on_TCP_Data(Device, p_Message);
		}
	#endif

	#ifdef CONFIG_SIM70XX_DRIVER_WITH_MQTT
		// Handle MQTT(S) socket publish events.
		if(p_Message->find("+CMQPUB") != std::string::npos)
		{
			SIM7020_Evt_on_MQTT_Pub(Device, p_Message);
		}

		// Handle MQTT(S) socket disconnect events.
		if(p_Message->find("+CMQDISCON") != std::string::npos)
		{
			SIM7020_Evt_on_MQTT_Disconnect(Device, p_Message);
		}
	#endif

	#ifdef CONFIG_SIM70XX_DRIVER_WITH_HTTP
		if(p_Message->find("+CHTTPERR") != std::string::npos)
		{
			SIM7020_Evt_on_HTTP_Error(Device, p_Message);
		}

		// Handle the HTTP header response.
		if(p_Message->find("+CHTTPNMIH") != std::string::npos)
		{
			SIM7020_Evt_on_HTTP_Header(Device, p_Message);
			Processed = true;
		}

		// Handle the HTTP data response.
		if(p_Message->find("+CHTTPNMIC") != std::string::npos)
		{
			SIM7020_Evt_on_HTTP_Data(Device, p_Message);
			Processed = true;
		}
	#endif

	if((Processed == true) || (xQueueSend(Device->Internal.EventQueue, &p_Message, 0) != pdPASS))
	{
		delete p_Message;
	}
}

#endif