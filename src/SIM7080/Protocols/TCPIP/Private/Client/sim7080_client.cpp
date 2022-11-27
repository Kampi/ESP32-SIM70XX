 /*
 * sim7080_tcpip_client.cpp
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

#if((CONFIG_SIMXX_DEV == 7080) && (defined CONFIG_SIM70XX_DRIVER_WITH_TCPIP))

#include "sim7080.h"
#include "sim7080_client.h"

#include "../../../../../Core/Queue/sim70xx_queue.h"
#include "../../../../../Core/Commands/sim70xx_commands.h"

#include "../../../../../Core/Arch/ESP32/UART/sim70xx_uart.h"
#include "../../../../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7080_TCPIP_Client";

SIM70XX_Error_t SIM7080_Client_CreateSocket(SIM7080_t& p_Device, SIM7080_TCP_Type_t Type, std::string IP, uint16_t Port, SIM7080_TCPIP_Socket_t* p_Socket, uint8_t CID, bool ReadManually)
{
    if(p_Socket == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if((CID > 12) || (ReadManually == true))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    p_Socket->IP = IP;
    p_Socket->Port = Port;
    p_Socket->Internal.CID = CID;
    p_Socket->Internal.Type = Type;
    p_Socket->Internal.isCreated = true;
    p_Socket->Internal.isConnected = false;
    p_Socket->Internal.isReadManually = ReadManually;
    p_Socket->Internal.isDataReceived = false;

    // TODO: Check if a socket with the ID is open

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_Client_ConnectSocket(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket, SIM7080_PDP_Context_t* p_PDP, SIM7080_TCP_Error_t* p_Result)
{
    std::string Type;
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM7080_TCP_Error_t Result;

    if((p_Socket == NULL) || (p_PDP == NULL) || (p_PDP->ID > 3))
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
    else if(p_Socket->Internal.isServer == true)
    {
        return SIM70XX_ERR_INVALID_SOCKET;
    }

    if(p_Socket->Internal.Type == SIM7080_TCP_TYPE_TCP)
    {
        Type = "TCP";
    }
    else if(p_Socket->Internal.Type == SIM7080_TCP_TYPE_UDP)
    {
        Type = "UDP";
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CAOPEN(p_Socket->Internal.CID, p_PDP->ID, Type, p_Socket->IP, p_Socket->Port);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    SIM70XX_Tools_SubstringSplitErase(&Response);
    Result = (SIM7080_TCP_Error_t)SIM70XX_Tools_StringToUnsigned(Response);

    if(p_Result != NULL)
    {
        *p_Result = Result;
    }

    SIM70XX_LOGI(TAG, "Connection status: %u", Result);

    if(Result != SIM7080_TCP_ERROR_OK)
    {
        return SIM70XX_ERR_FAIL;
    }

    p_Socket->Internal.isConnected = true;
    p_Device.TCP.Sockets.push_back(p_Socket);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_Client_Transmit(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket, const void* p_Buffer, uint32_t Length, uint8_t Retries, uint16_t Timeout, uint16_t PacketSize)
{
    uint8_t RetryCounter;
    uint8_t* Buffer = (uint8_t*)p_Buffer;
    uint32_t Remaining = Length;
    SIM70XX_Error_t Error = SIM70XX_ERR_OK;

    if((p_Socket == NULL) || ((p_Buffer == NULL) && (Length > 0)) || (PacketSize > SIM7080_TCP_MAX_PAYLOAD_SIZE))
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
    else if(Length == 0)
    {
        return SIM70XX_ERR_OK;
    }
    else if(p_Socket->Internal.isServer == true)
    {
        return SIM70XX_ERR_INVALID_SOCKET;
    }

    // NOTE: We can not use the standard process here, because the response (">") does not contain a new line. The command will end with an empty space (0x20).
    vTaskSuspend(p_Device.UART.TaskHandle);
    SIM70XX_LOGI(TAG, "Total %u bytes to transmit...", Remaining);
    RetryCounter = 0;
    do
    {
        uint32_t BytesToSend;
        std::string Response;
        SIM70XX_TxCmd_t Command;

        if(Remaining > PacketSize)
        {
            BytesToSend = PacketSize;
        }
        else
        {
            BytesToSend = Remaining;
        }

        SIM70XX_LOGI(TAG, "     Transmit %u bytes...", BytesToSend);

        // Transmit the command.
        Command = SIM7080_AT_CASEND(p_Socket->Internal.CID, BytesToSend, Timeout);
        SIM70XX_UART_SendLine(p_Device.UART, Command.Command);

        // Wait for the empty space after the ">".
        Response = SIM70XX_UART_ReadStringUntil(p_Device.UART, ' ', Command.Timeout * 1000UL);
        if(Response.find(">") == std::string::npos)
        {
            SIM70XX_LOGE(TAG, "Invalid response. Expect '>', got: %s", Response.c_str());

            Error = SIM70XX_ERR_FAIL;
            break;
        }

        // Send the data.
        SIM70XX_UART_Send(p_Device.UART, Buffer, BytesToSend);

        // Read and parse the status code.
        SIM70XX_UART_ReadStringUntil(p_Device.UART);
        Response = SIM70XX_UART_ReadStringUntil(p_Device.UART, '\n', Command.Timeout * 1000UL);

        ESP_LOGW(TAG, "Response: %s", Response.c_str());
    
        // Transmission error. Repeat the last packet.
        if(Response.find("ERROR") != std::string::npos)
        {
            if(RetryCounter < Retries)
            {
                RetryCounter++;

                // Give a short break to allow the modem to handle the error.
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                continue;
            }
            else
            {
                Error = SIM70XX_ERR_TIMEOUT;
                break;
            }
        }
        // Data received. Abort the transmission.
        else if(Response.find("+CADATAIND") != std::string::npos)
        {
            p_Socket->Internal.isDataReceived = true;
            Error = SIM70XX_ERR_FAIL;
            break;
        }
        // Response "OK" - transmit the next packet.
        else if(Response.find("OK") != std::string::npos)
        {
            RetryCounter = 0;
            Buffer += BytesToSend;
            Remaining -= BytesToSend;
        }
    } while(Remaining > 0);
    vTaskResume(p_Device.UART.TaskHandle);

    return Error;
}

SIM70XX_Error_t SIM7080_Client_Receive(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket, std::string* p_Buffer, uint32_t Length)
{
    SIM70XX_TxCmd_t Command;

    if((p_Socket == NULL) || (p_Buffer == NULL))
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
    else if(p_Socket->Internal.isDataReceived == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    else if(p_Socket->Internal.isServer == true)
    {
        return SIM70XX_ERR_INVALID_SOCKET;
    }

    // We have to use the event queue here, because payload can contain CR and LF which will lead to wrong results when we use the normal way here.
    SIM70XX_LOGI(TAG, "Transmit command: %s", Command.Command.c_str());
    vTaskSuspend(p_Device.UART.TaskHandle);
    Command = SIM7080_AT_CARECV(p_Socket->Internal.CID, Length);
    SIM70XX_UART_SendLine(p_Device.UART, Command.Command);
    vTaskResume(p_Device.UART.TaskHandle);

    while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+CARECV" , p_Buffer) == false)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    // Remove the command and the length from the response.
    SIM70XX_Tools_SubstringSplitErase(p_Buffer);

    p_Socket->Internal.isDataReceived = false;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_Client_DisconnectSocket(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket)
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
    else if(p_Socket->Internal.isServer == true)
    {
        return SIM70XX_ERR_INVALID_SOCKET;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CACLOSE(p_Socket->Internal.CID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    p_Socket->Internal.isConnected = false;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_Client_DestroySocket(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket)
{
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
    else if(p_Socket->Internal.isServer == true)
    {
        return SIM70XX_ERR_INVALID_SOCKET;
    }

    p_Socket->Internal.isCreated = false;

    return SIM70XX_ERR_OK;
}

#ifdef CONFIG_SIM70XX_DRIVER_WITH_SSL
    SIM70XX_Error_t SIM7080_Client_EnableSSL(SIM7080_t& p_Device, bool Enable, uint8_t CID)
    {
        SIM70XX_TxCmd_t* Command;
        std::string CommandStr;

        if(CID > 12)
        {
            return SIM70XX_ERR_INVALID_ARG;
        }
        else if(p_Device.Internal.isInitialized == false)
        {
            return SIM70XX_ERR_NOT_INITIALIZED;
        }

        CommandStr = "AT+CASSLCFG=" + std::to_string(CID) + ",\"SSL\"," + std::to_string(Enable);

        SIM70XX_CREATE_CMD(Command);
        *Command = SIM7080_AT_CASSLCFG(CommandStr);
        SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
        if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
        {
            return SIM70XX_ERR_FAIL;
        }
        SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

        return SIM70XX_ERR_OK;
    }
#endif

#endif