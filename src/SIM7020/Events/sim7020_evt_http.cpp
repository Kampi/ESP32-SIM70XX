 /*
 * SIM7020_evt_http.cpp
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

#include "sim7020.h"
#include "sim7020_evt.h"

#include "../../Core/Queue/sim70xx_queue.h"

#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

#include "../../Core/Arch/ESP32/UART/sim70xx_uart.h"

static const char* TAG = "SIM7020_Evt_HTTP";

void SIM7020_Evt_on_HTTP_Error(SIM7020_t* const p_Device, std::string* p_Message)
{
    uint8_t ID;
    size_t Index;
    SIM7020_HTTP_Error_t HTTP_Error;
    std::string Message;

    SIM70XX_LOGI(TAG, "HTTP error event!");

    // Remove the command and make a copy of the command response.
    Index = p_Message->find("+CHTTPERR: ");
    if(Index == std::string::npos)
    {
        return;
    }
    Message = p_Message->substr(Index + std::string("+CHTTPERR: ").size());
    SIMXX_TOOLS_REMOVE_LINEEND((Message));

    Index = p_Message->find(",");

    ID = (uint8_t)SIM70XX_Tools_StringToUnsigned(p_Message->substr(Index - 1, 1));
    HTTP_Error = (SIM7020_HTTP_Error_t)SIM70XX_Tools_StringToUnsigned(p_Message->substr(Index + 1));

    // Iterate through the list of active sockets and close the socket with the given ID.
    for(std::vector<SIM7020_HTTP_Socket_t*>::iterator it = p_Device->HTTP.Sockets.begin(); it != p_Device->HTTP.Sockets.end(); ++it)
    {
        if((*it)->Internal.ID == ID)
        {
            (*it)->Internal.isConnected = false;

            SIM70XX_LOGD(TAG, "Disconnect socket %u", ID);
            SIM70XX_LOGD(TAG, "Error: %i", HTTP_Error);
        }
    }
}

void SIM7020_Evt_on_HTTP_Header(SIM7020_t* const p_Device, std::string* p_Message)
{
    bool Flag;
    size_t Index;
    std::string Message;
    SIM7020_HTTP_Response_t* Response;

    Response = new SIM7020_HTTP_Response_t();

    SIM70XX_LOGI(TAG, "HTTP header event!");

    // Remove the command and make a copy of the command response.
    Index = p_Message->find("+CHTTPNMIH: ");
    if(Index == std::string::npos)
    {
        return;
    }
    Message = p_Message->substr(Index + std::string("+CHTTPNMIH: ").size());

    // Get the socket ID from the response.
    Response->ID = (uint8_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Message));

    // Get the response code from the response.
    Response->ResponseCode = (uint16_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Message));

    // Remove the header length from the response.
    SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Message));

    // Split the header from the response.
    Response->Header = SIM70XX_Tools_SubstringSplitErase(&Message, "\r\n\r\n");

    // Process the content.
    do
    {
        // Remove the command.
        SIM70XX_Tools_SubstringSplitErase(&Message);

        // Get the flag.
        Flag = (bool)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Message));

        // Remove the total length.
        SIM70XX_Tools_SubstringSplitErase(&Message);

        // Remove the package length.
        SIM70XX_Tools_SubstringSplitErase(&Message);

        // Continue when the flag is set (additional packets are available).
        if(Flag == true)
        {
            Response->Content += SIM70XX_Tools_SubstringSplitErase(&Message, "\r\n\r\n");
        }
        // Otherwise receive the remaining data and exit.
        else
        {
            Response->Content += SIM70XX_Tools_SubstringSplitErase(&Message, "\r\n");

            break;
        }
    } while(true);

    SIM70XX_LOGD(TAG, "ID: %u", Response->ID);
    SIM70XX_LOGD(TAG, "Header: %s", Response->Header.c_str());
    SIM70XX_LOGD(TAG, "Response code: %u", Response->ResponseCode);
    SIM70XX_LOGD(TAG, "Content: %s", Response->Content.c_str());

    for(std::vector<SIM7020_HTTP_Socket_t*>::iterator it = p_Device->HTTP.Sockets.begin(); it != p_Device->HTTP.Sockets.end(); ++it)
    {
        if((*it)->Internal.ID == Response->ID)
        {
            if((*it)->Internal.ResponseQueue == NULL)
            {
                xQueueSend((*it)->Internal.ResponseQueue, &Response, 0);
            }
        }
    }
}

void SIM7020_Evt_on_HTTP_Data(SIM7020_t* const p_Device, std::string* p_Message)
{

}

#endif