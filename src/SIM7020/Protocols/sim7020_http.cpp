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

#include <esp_log.h>

#include "sim7020.h"
#include "sim7020_http.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

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

    // Check if the URL is valid.
    std::for_each(p_Socket->Host.begin(), p_Socket->Host.end(), [](char& c)
    {
        c = ::tolower(c);
    });
    if((p_Socket->Host.find("http") == std::string::npos) && (p_Socket->Host.find("https") == std::string::npos))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CHTTPCREATE(p_Socket->Host);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, p_Socket->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    p_Socket->isConnected = false;
    p_Socket->isCreated = true;
    p_Socket->ID = (uint8_t)std::stoi(Response);

    ESP_LOGI(TAG, "Socket %u created...", p_Socket->ID);

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
    else if(p_Socket->isCreated == false)
    {
        return SIM70XX_ERR_NOT_CREATED;
    }

    p_Socket->isConnected = false;
    p_Socket->Timeout = Timeout;

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CHTTCON(p_Socket->ID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, p_Socket->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response, &Status));

    // Everything okay. The socket is active now.
    ESP_LOGI(TAG, "Socket %u opened...", p_Socket->ID);

    p_Device.HTTP.Sockets.push_back(p_Socket);
    p_Socket->isConnected = true;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_HTTP_POST(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket, std::string Path, std::string ContentType, std::string Header, std::string Payload, uint16_t* p_ResponseCode)
{
    return SIM7020_HTTP_POST(p_Device, p_Socket, Path, ContentType, Header, Payload.c_str(), Payload.size(), p_ResponseCode);
}

SIM70XX_Error_t SIM7020_HTTP_POST(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket, std::string Path, std::string ContentType, std::string Header, const void* p_Buffer, uint32_t Length, uint16_t* p_ResponseCode)
{
    std::string CommandStr;
    std::string Buffer_Hex;
    std::string Packet;
    uint16_t ResponseCode;
    uint32_t Now;
    uint32_t PayloadLength_Length;
    uint32_t TotalLength;
    uint32_t Length_Temp = Length;
    SIM70XX_TxCmd_t* Command;
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
    else if(p_Socket->isConnected == false)
    {
        return SIM70XX_ERR_NOT_CONNECTED;
    }

    // Prepare the header packet.
    SIM70XX_Tools_ASCII2Hex(Header.c_str(), Header.size(), &Buffer_Hex);
    Packet = std::to_string(p_Socket->ID) + "," +
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

        SIM70XX_Tools_ASCII2Hex(Buffer_Temp, BytesToTransmit, &Buffer_Hex);

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

        Length_Temp -= BytesToTransmit;
        Buffer_Temp += BytesToTransmit;

        vTaskDelay(10 / portTICK_PERIOD_MS);
    } while(Length_Temp != 0);

    // Get the response from the server.
    Now = SIM70XX_Tools_GetmsTimer();
    while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+CHTTPNMIH: " + std::to_string(p_Socket->ID), &Packet) == false)
    {
        if((SIM70XX_Tools_GetmsTimer() - Now) > (p_Socket->Timeout * 1000UL))
        {
            return SIM70XX_ERR_TIMEOUT;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    // Remove the command from the response.
    Packet.replace(Packet.find("+CHTTPNMIH: " + std::to_string(p_Socket->ID) + ","), std::string("+CHTTPNMIH: " + std::to_string(p_Socket->ID) + ",").size(), "");

    // Get the response code.
    ResponseCode = std::stoi(Packet.substr(0, Packet.find(",")));

    if(p_ResponseCode != NULL)
    {
        *p_ResponseCode = ResponseCode;
    }

    // TODO: Items in Queue müssen korrekt ausgewertet werden
    SIM70XX_Queue_Reset(p_Device.Internal.RxQueue);
    ESP_LOGI(TAG, "Items: %u", SIM70XX_Queue_GetItems(p_Device.Internal.RxQueue));

    ESP_LOGI(TAG, "Response from server: %s", Packet.c_str());
    ESP_LOGI(TAG, "     Code: %u", ResponseCode);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_HTTP_GET(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket, std::string Path, uint8_t** p_Buffer, uint32_t* p_Length, uint16_t* p_ResponseCode)
{
    size_t Index;
    uint32_t Now;
    uint16_t ResponseCode;
    std::string Response;
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;
    bool isAdditionalData;

    if((p_Socket == NULL) || (p_Buffer == NULL) || (p_Length == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Socket->isConnected == false)
    {
        return SIM70XX_ERR_OK;
    }

    CommandStr = "AT+CHTTPSEND=" + std::to_string(p_Socket->ID) + "," +
                                   std::to_string(SIM7020_HTTP_REQ_GET) + "," +
                                   "\"" + Path + "\"";
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CHTTPSEND(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, p_Socket->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);

    // Get the response from the server.
    Now = SIM70XX_Tools_GetmsTimer();
    while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+CHTTPNMIH: " + std::to_string(p_Socket->ID), &Response) == false)
    {
        if((SIM70XX_Tools_GetmsTimer() - Now) > (p_Socket->Timeout * 1000UL))
        {
            return SIM70XX_ERR_TIMEOUT;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    // Remove the command from the response.
    Index = Response.find("+CHTTPNMIH: " + std::to_string(p_Socket->ID) + ",");
    Response.replace(Index, std::string("+CHTTPNMIH: " + std::to_string(p_Socket->ID) + ",").size(), "");

    // Get the response code.
    ResponseCode = std::stoi(Response.substr(0, Response.find(",")));

    // Remove the header and the command from the response.
    Index = Response.find("+CHTTPNMIC: " + std::to_string(p_Socket->ID) + ",");
    Response = Response.substr(Index + std::string("+CHTTPNMIC: " + std::to_string(p_Socket->ID) + ",").size());

    // Get the additional data flag.
    // TODO: Must be implemented
    isAdditionalData = (bool)std::stoi(SIM70XX_Tools_SubstringSplitErase(&Response));

    // Remove the total length.
    SIM70XX_Tools_SubstringSplitErase(&Response);

    // Get the payload length.
    *p_Length = (uint32_t)std::stoi(SIM70XX_Tools_SubstringSplitErase(&Response));

    *p_Buffer = (uint8_t*)malloc(*p_Length);

    // Get the hexadecimal payload string.
    SIM70XX_Tools_Hex2ASCII(Response, *p_Buffer);

    if(p_ResponseCode != NULL)
    {
        *p_ResponseCode = ResponseCode;
    }

    ESP_LOGI(TAG, "Response code: %u", ResponseCode);
    ESP_LOGI(TAG, "Additional data: %u", isAdditionalData);
    ESP_LOGI(TAG, "Length: %u", *p_Length);
    ESP_LOGI(TAG, "Response: %s", (char*)p_Buffer);

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
    else if(p_Socket->isConnected == false)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CHTTPDISCON(p_Socket->ID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, p_Socket->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    // Remove the socket from the list with active sockets.
    for(std::vector<SIM7020_HTTP_Socket_t*>::iterator it = p_Device.HTTP.Sockets.begin(); it != p_Device.HTTP.Sockets.end(); ++it)
    {
        if((*it)->ID == p_Socket->ID)
        {
            p_Device.HTTP.Sockets.erase(it);

            break;
        }
    }

    p_Socket->isConnected = false;

    ESP_LOGI(TAG, "Socket %u closed...", p_Socket->ID);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_HTTP_DestroyAllSockets(SIM7020_t& p_Device )
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    for(uint8_t i = 0; i < 5; i++)
    {
        SIM70XX_CREATE_CMD(Command);
        *Command = SIM7020_AT_CHTTPDISCON(i);
        SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
        if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
        {
            return SIM70XX_ERR_FAIL;
        }
        SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));
    }

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

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CHTTPDESTROY(p_Socket->ID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, p_Socket->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response, NULL));

    ESP_LOGI(TAG, "Socket %u destroyed...", p_Socket->ID);

    return SIM70XX_ERR_OK;
}

void SIM7020_HTTP_AddToHeader(std::string Key, std::string Value, std::string* p_Header)
{
    if(p_Header == NULL)
    {
        return;
    }

    p_Header->append(Key);
    p_Header->append(": ");
    p_Header->append(Value);
    p_Header->append("\n");
}

#endif