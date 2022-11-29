 /*
 * sim7020_http.cpp
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
#include "sim7020_http.h"

#include "../../Core/Queue/sim70xx_queue.h"
#include "../../Core/Commands/sim70xx_commands.h"

#include "../../Core/Arch/ESP32/Timer/sim70xx_timer.h"
#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7020_HTTP";

SIM70XX_Error_t SIM7020_HTTP_Create(SIM7020_t& p_Device, std::string Host, SIM7020_HTTP_Socket_t* p_Socket)
{
    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    p_Socket->Host = Host;
    p_Socket->Timeout = 60;

    return SIM7020_HTTP_Create(p_Device, p_Socket);
}

SIM70XX_Error_t SIM7020_HTTP_Create(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->Internal.isCreated == true)
    {
        return SIM70XX_ERR_OK;
    }
    else if(p_Socket->Internal.isConnected == true)
    {
        return SIM70XX_ERR_OK;
    }

    // Check if the URL is valid.
    std::for_each(p_Socket->Host.begin(), p_Socket->Host.end(), [](char& c)
    {
        c = std::tolower(c);
    });
    if((p_Socket->Host.find("http") == std::string::npos) && (p_Socket->Host.find("https") == std::string::npos))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CHTTPCREATE(p_Socket->Host);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, p_Socket->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    p_Socket->Internal.isConnected = false;
    p_Socket->Internal.isCreated = true;
    p_Socket->Internal.ID = static_cast<uint8_t>(SIM70XX_Tools_StringToUnsigned(Response));

    SIM70XX_LOGI(TAG, "Socket %u created...", p_Socket->Internal.ID);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_HTTP_Connect(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket, uint16_t Timeout)
{
    std::string Status;
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->Internal.isCreated == false)
    {
        return SIM70XX_ERR_SOCKET_NOT_CREATED;
    }
    else if(p_Socket->Internal.isConnected == true)
    {
        return SIM70XX_ERR_OK;
    }

    p_Socket->Internal.isConnected = false;
    p_Socket->Timeout = Timeout;

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CHTTCON(p_Socket->Internal.ID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, p_Socket->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response, &Status));

    if(p_Socket->Internal.ResponseQueue == NULL)
    {
        p_Socket->Internal.ResponseQueue = xQueueCreate(CONFIG_SIM70XX_QUEUE_LENGTH, sizeof(SIM7020_HTTP_Response_t*));
    }

    // Everything okay. The socket is active now.
    SIM70XX_LOGI(TAG, "Socket %u opened...", p_Socket->Internal.ID);

    p_Device.HTTP.Sockets.push_back(p_Socket);
    p_Socket->Internal.isConnected = true;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_HTTP_POST(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket, std::string Path, std::string ContentType, std::string Header, std::string Payload, SIM7020_HTTP_Response_t* p_Response)
{
    return SIM7020_HTTP_POST(p_Device, p_Socket, Path, ContentType, Header, Payload.c_str(), Payload.size(), p_Response);
}

SIM70XX_Error_t SIM7020_HTTP_POST(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket, std::string Path, std::string ContentType, std::string Header, const void* p_Buffer, uint32_t Length, SIM7020_HTTP_Response_t* p_Response)
{
    std::string CommandStr;
    std::string Buffer_Hex;
    std::string Packet;
    uint32_t Now;
    uint32_t PayloadLength_Length;
    uint32_t TotalLength;
    uint32_t Length_Temp = Length;
    SIM70XX_TxCmd_t* Command;
    SIM7020_HTTP_Response_t Response;
    bool isFirstPacket;
    char* Buffer_Temp = (char*)p_Buffer;

    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->Internal.isCreated == false)
    {
        return SIM70XX_ERR_SOCKET_NOT_CREATED;
    }
    else if(p_Socket->Internal.isConnected == false)
    {
        return SIM70XX_ERR_NOT_CONNECTED;
    }

    // Prepare the header packet.
    SIM70XX_Tools_Buf2Hex(Header.c_str(), Header.size(), &Buffer_Hex);
    Packet = std::to_string(p_Socket->Internal.ID) + "," +
             std::to_string(SIM7020_HTTP_REQ_POST) + "," +
             std::to_string(Path.size()) + "," +
             "\"" + Path + "\"" + "," +
             std::to_string(Buffer_Hex.size()) + "," +
             Buffer_Hex + "," +
             std::to_string(ContentType.size()) + "," +
             "\"" + ContentType + "\",";

    // The length of the payload length and the trailing comma.
    PayloadLength_Length = std::to_string(Length_Temp * 2).size() + 1;

    // The total length is calculated with 
    //  - The length of the first packet, beginning with the socket ID  +
    //  - The length of the payload (multiplied by 2, because ASCII)    +
    //  - The length of the payload length                              +
    //  - 1 (for the comma)
    TotalLength = Packet.size() + (Length_Temp * 2) + PayloadLength_Length;

    // Transmit the header.
    CommandStr = "AT+CHTTPSENDEXT=" + std::to_string(1) + "," +
                                      std::to_string(TotalLength) + "," +
                                      std::to_string(Packet.size()) + "," +
                                      Packet;
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CHTTPSENDEXT(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, p_Socket->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    // Transmit the data packets.
    isFirstPacket = true;
    do
    {
        uint32_t BytesToTransmit;
        bool isAdditionalPackets;

        if(Length_Temp > (SIM7020_CMD_BUFFER / 4))
        {
            BytesToTransmit = (SIM7020_CMD_BUFFER / 4);
            isAdditionalPackets = true;
        }
        else
        {
            BytesToTransmit = Length_Temp;
            isAdditionalPackets = false;
        }

        SIM70XX_Tools_Buf2Hex(Buffer_Temp, BytesToTransmit, &Buffer_Hex);

        if(isFirstPacket == true)
        {
            // Prepare the payload packet
            //  <Length>,<Payload>
            Packet = std::to_string(Length_Temp * 2) + "," + Buffer_Hex;

            // NOTE: The first data packet must contain the length of the payload.
            //  <1/0>, <TotalLength>,<Payload Length + Number of Chars for the Payload length + 1>,<Payload Length>
            CommandStr = "AT+CHTTPSENDEXT=" + std::to_string(isAdditionalPackets) + "," +
                                              std::to_string(TotalLength) + "," +
                                              std::to_string(Buffer_Hex.size() + PayloadLength_Length) + "," +
                                              Packet;

            isFirstPacket = false;
        }
        else
        {
            CommandStr = "AT+CHTTPSENDEXT=" + std::to_string(isAdditionalPackets) + "," +
                                              std::to_string(TotalLength) + "," +
                                              std::to_string(Buffer_Hex.size()) + "," + Buffer_Hex;
        }

        SIM70XX_CREATE_CMD(Command);
        *Command = SIM7020_AT_CHTTPSENDEXT(CommandStr);
        SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
        if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, p_Socket->Timeout) == false)
        {
            return SIM70XX_ERR_FAIL;
        }
        SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

        Length_Temp -= BytesToTransmit;
        Buffer_Temp += BytesToTransmit;

        vTaskDelay(10 / portTICK_PERIOD_MS);
    } while((Length_Temp != 0) && (p_Socket->Internal.isConnected == true));

    Now = SIM70XX_Timer_GetMilliseconds();
    while(SIM7020_HTTP_GetResponseItems(p_Socket) == 0)
    {
        if((SIM70XX_Timer_GetMilliseconds() - Now) > (p_Socket->Timeout * 1000UL))
        {
            return SIM70XX_ERR_TIMEOUT;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    SIM70XX_ERROR_CHECK(SIM7020_HTTP_GetResponse(p_Device, p_Socket, &Response));

    SIM70XX_LOGI(TAG, "Got Response from server...");
    SIM70XX_LOGI(TAG, "     Response code: %u", Response.ResponseCode);

    if(p_Response != NULL)
    {
        *p_Response = Response;
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_HTTP_GET(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket, std::string Path)
{
    std::string Response;
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->Internal.isCreated == false)
    {
        return SIM70XX_ERR_SOCKET_NOT_CREATED;
    }
    else if(p_Socket->Internal.isConnected == false)
    {
        return SIM70XX_ERR_NOT_CONNECTED;
    }

    CommandStr = "AT+CHTTPSEND=" + std::to_string(p_Socket->Internal.ID) + "," +
                                   std::to_string(SIM7020_HTTP_REQ_GET) + "," +
                                   "\"" + Path + "\"";
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CHTTPSEND(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, p_Socket->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_HTTP_GetResponse(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket, SIM7020_HTTP_Response_t* p_Response)
{
    SIM7020_HTTP_Response_t* Packet;

    if((p_Response == NULL) || (p_Socket->Internal.ResponseQueue == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(uxQueueMessagesWaiting(p_Socket->Internal.ResponseQueue) == 0)
    {
        return SIM70XX_ERR_QUEUE_EMPTY;
    }

    if(xQueueReceive(p_Socket->Internal.ResponseQueue, &Packet, 0) != pdTRUE)
    {
        return SIM70XX_ERR_QUEUE_ERR;
    }

    *p_Response = *Packet;

    delete Packet;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_HTTP_Disconnect(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->Internal.isCreated == false)
    {
        return SIM70XX_ERR_SOCKET_NOT_CREATED;
    }
    else if(p_Socket->Internal.isConnected == false)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CHTTPDISCON(p_Socket->Internal.ID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, p_Socket->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    if(p_Socket->Internal.ResponseQueue != NULL)
    {
        vQueueDelete(p_Socket->Internal.ResponseQueue);
    }

    // Remove the socket from the list with active sockets.
    for(std::vector<SIM7020_HTTP_Socket_t*>::iterator it = p_Device.HTTP.Sockets.begin(); it != p_Device.HTTP.Sockets.end(); ++it)
    {
        if((*it)->Internal.ID == p_Socket->Internal.ID)
        {
            p_Device.HTTP.Sockets.erase(it);

            break;
        }
    }

    p_Socket->Internal.isConnected = false;

    SIM70XX_LOGI(TAG, "Socket %u closed...", p_Socket->Internal.ID);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_HTTP_Destroy(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->Internal.isCreated == false)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CHTTPDESTROY(p_Socket->Internal.ID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, p_Socket->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response, NULL));

    SIM70XX_LOGI(TAG, "Socket %u destroyed...", p_Socket->Internal.ID);

    return SIM70XX_ERR_OK;
}

#endif