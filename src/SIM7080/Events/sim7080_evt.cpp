 /*
 * sim7080_evt.cpp
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

#if(CONFIG_SIMXX_DEV == 7080)

#include "sim7080.h"
#include "sim7080_evt.h"

#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7080_Event";

void SIM70XX_Evt_MessageFilter(void* p_Device, std::string* const p_Message)
{
	// This flag is set to #true when the data were processed completely in the event handler. Do not set this flag to #true when
	// the data should be processed by using the event message queue.
	bool Processed;
	SIM7080_t* Device = (SIM7080_t*)p_Device;

	Processed = false;

	if(p_Message->find("+CPIN: READY") != std::string::npos)
	{
		SIM70XX_LOGI(TAG, "CPIN event");

		Device->Connection.Functionality = SIM7080_FUNC_FULL;

		Processed = true;
	}

	if(p_Message->find("+CPIN: NOT READY") != std::string::npos)
	{
		SIM70XX_LOGI(TAG, "CPIN event");

		Device->Connection.Functionality = SIM7080_FUNC_MIN;

		Processed = true;
	}

	// Shutdown message was received.
	if(p_Message->find("NORMAL POWER DOWN") != std::string::npos)
	{
		SIM70XX_LOGI(TAG, "Power down event!");

		Device->Internal.isInitialized = false;
		Processed = true;
	}

	// PSM related events.
	if(p_Message->find("+CPSMSTATUS") != std::string::npos)
	{
		SIM70XX_LOGI(TAG, "PSM event!");

		SIM7080_Evt_on_PSM(Device, p_Message);
		Processed = true;
	}

	#ifdef CONFIG_SIM70XX_DRIVER_WITH_GNSS
		if(p_Message->find("+SGNSCMD") != std::string::npos)
		{
			SIM7080_Evt_on_GNSS(Device, p_Message);
			Processed = true;
		}
	#endif

	#ifdef CONFIG_SIM70XX_DRIVER_WITH_TCPIP
		// NOTE: The modem can output the event messages combined. So we have to search and process each message one by one.
		if(p_Message->find("+CASTATE") != std::string::npos)
		{
			SIM7080_Evt_on_TCP_Disconnect(Device, p_Message);
			Processed = true;
		}

		if(p_Message->find("+CADATAIND") != std::string::npos)
		{
			SIM7080_Evt_on_TCP_DataReady(Device, p_Message);
			Processed = true;
		}

		if(p_Message->find("+CARECV") != std::string::npos)
		{
			SIM7080_Evt_on_TCP_Data(Device, p_Message);
			Processed = true;
		}
	#endif

	#ifdef CONFIG_SIM70XX_DRIVER_WITH_MQTT
		if(p_Message->find("+SMSUB") != std::string::npos)
		{
			SIM7080_Evt_on_MQTT_Subscribe(Device, p_Message);
			Processed = true;
		}
	#endif

	// Delete the message object when the message was completely processed by the event handler or
	// when the message can??t be pushed to the event queue.
	if((Processed == true) || (xQueueSend(Device->Internal.EventQueue, &p_Message, 0) != pdPASS))
	{
		SIM70XX_LOGI(TAG, "Items in event queue: %u", uxQueueMessagesWaiting(Device->Internal.EventQueue));

		delete p_Message;
	}
}

#endif