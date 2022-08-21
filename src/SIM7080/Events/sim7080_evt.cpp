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

void SIM70XX_Evt_MessageFilter(void* p_Device, std::string* p_Message)
{
	bool Found;
	SIM7080_t* Device = (SIM7080_t*)p_Device;

	Found = false;

	// Shutdown message was received.
	if(p_Message->find("NORMAL POWER DOWN") != std::string::npos)
	{
	}

	#ifdef CONFIG_SIM70XX_DRIVER_WITH_TCPIP
		if(p_Message->find("+CASTATE") != std::string::npos)
		{
			SIM7080_Evt_on_TCP_Disconnect(Device, p_Message);
			Found = true;
		}

		if(p_Message->find("+CADATAIND") != std::string::npos)
		{
			SIM7080_Evt_on_TCP_DataReady(Device, p_Message);
			Found = true;
		}
	#endif

	#ifdef CONFIG_SIM70XX_DRIVER_WITH_EMAIL
	#endif

	// Handle all other messages by putting them into the event queue.
	if(Found == false)
	{
		xQueueSend(Device->Internal.EventQueue, &p_Message, 0);
	}
	else
	{
		delete p_Message;
	}
}

#endif