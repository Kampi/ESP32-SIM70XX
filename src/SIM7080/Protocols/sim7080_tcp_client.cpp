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

#include <esp_log.h>

#include "sim7080.h"
#include "sim7080_tcpip.h"
#include "../../Private/UART/sim70xx_uart.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

/** @brief Maximum number of bytes for a single TCP / UDP transmission.
 */
#define SIM7080_TCP_MAX_PAYLOAD_SIZE                        1460

static const char* TAG = "SIM7080_TCPIP";

SIM70XX_Error_t SIM7080_TCP_Client_Create(SIM7080_t& p_Device, std::string IP, uint16_t Port, SIM7080_TCP_Socket_t* p_Socket, uint8_t CID, bool ReadManually)
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
    p_Socket->ID = CID;
    p_Socket->Type = SIM7080_TCP_TYPE_TCP;
    p_Socket->isCreated = true;
    p_Socket->isConnected = false;
    p_Socket->isReadManually = ReadManually;
    p_Socket->isDataReceived = false;

    // TODO: Check if a socket with the ID is open

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_TCP_Client_Connect(SIM7080_t& p_Device, SIM7080_TCP_Socket_t* p_Socket, uint8_t PDP, SIM7080_TCP_Error_t* p_Result)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM7080_TCP_Error_t Result;

    if((p_Socket == NULL) || (PDP > 3))
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
    else if(p_Socket->isConnected == true)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CAOPEN(p_Socket->ID, PDP, "TCP", p_Socket->IP, p_Socket->Port);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    SIM70XX_Tools_SubstringSplitErase(&Response);
    Result = (SIM7080_TCP_Error_t)std::stoi(Response);

    if(p_Result != NULL)
    {
        *p_Result = Result;
    }

    ESP_LOGI(TAG, "Connection status: %u", Result);

    if(Result != SIM7080_TCP_ERROR_OK)
    {
        return SIM70XX_ERR_FAIL;
    }

    p_Socket->isConnected = true;
    p_Device.TCP.Sockets.push_back(p_Socket);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_TCP_Client_println(SIM7080_t& p_Device, SIM7080_TCP_Socket_t* p_Socket, std::string Message)
{
    std::string Data = Message + "\r\n";

    return SIM7080_TCP_Client_Transmit(p_Device, p_Socket, Data.c_str(), Data.size());
}

SIM70XX_Error_t SIM7080_TCP_Client_Transmit(SIM7080_t& p_Device, SIM7080_TCP_Socket_t* p_Socket, const void* p_Buffer, uint32_t Length)
{
    uint8_t* Buffer = (uint8_t*)p_Buffer;
    uint32_t Remaining = Length;
    SIM70XX_Error_t Error = SIM70XX_ERR_OK;
    SIM70XX_TxCmd_t* Command;

    if((p_Socket == NULL) || (p_Socket->Type != SIM7080_TCP_TYPE_TCP) || ((p_Buffer == NULL) && (Length > 0)))
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
    else if(p_Socket->isConnected == false)
    {
        return SIM70XX_ERR_NOT_CONNECTED;
    }
    else if(Length == 0)
    {
        return SIM70XX_ERR_OK;
    }

    ESP_LOGI(TAG, "Total %u bytes to transmit...", Remaining);
    do
    {
        uint32_t BytesToSend;
        std::string Response;

        if(Remaining > SIM7080_TCP_MAX_PAYLOAD_SIZE)
        {
            BytesToSend = SIM7080_TCP_MAX_PAYLOAD_SIZE;
        }
        else
        {
            BytesToSend = Remaining;
        }

        ESP_LOGI(TAG, "     Transmit %u bytes...", BytesToSend);

        // NOTE: We can not use the standard process here, because the response (">") does not contain a new line. The command will end with an empty space (0x20).
        vTaskSuspend(p_Device.Internal.TaskHandle);
        SIM70XX_CREATE_CMD(Command);
        *Command = SIM7080_AT_CASEND(p_Socket->ID, BytesToSend);
        SIM70XX_UART_SendLine(p_Device.UART, Command->Command);

        // Wait for the empty space after the ">".
        Response = SIM70XX_UART_ReadStringUntil(p_Device.UART, ' ', Command->Timeout * 1000UL);
        if(Response.find(">") == std::string::npos)
        {
            ESP_LOGE(TAG, "Invalid response. Expect '>', got: %s", Response.c_str());

            Error = SIM70XX_ERR_FAIL;
            goto SIM7080_TCP_Client_Transmit_Exit;
        }

        // Send the data.
        SIM70XX_UART_Send(p_Device.UART, Buffer, BytesToSend);

        // Read the trailing "OK".
        SIM70XX_UART_ReadStringUntil(p_Device.UART);
        Response = SIM70XX_UART_ReadStringUntil(p_Device.UART, '\n', Command->Timeout * 1000UL);
        if(Response.find("OK") == std::string::npos)
        {
            ESP_LOGE(TAG, "Invalid response. Expect 'OK', got: %s", Response.c_str());

            Error = SIM70XX_ERR_FAIL;
            goto SIM7080_TCP_Client_Transmit_Exit;
        }

        Buffer += BytesToSend;
        Remaining -= BytesToSend;
    } while((Remaining > 0) && (p_Socket->isConnected));

SIM7080_TCP_Client_Transmit_Exit:
    delete Command;

    vTaskResume(p_Device.Internal.TaskHandle);

    return Error;
}

SIM70XX_Error_t SIM7080_TCP_Client_Receive(SIM7080_t& p_Device, SIM7080_TCP_Socket_t* p_Socket, uint32_t Length, void* p_Buffer, uint32_t* p_BytesRead)
{
    uint32_t BytesRead = 0;
    uint8_t* Buffer = (uint8_t*)p_Buffer;
    SIM70XX_TxCmd_t* Command;

    if((p_Socket == NULL) || (p_Socket->Type != SIM7080_TCP_TYPE_TCP) || (p_Buffer == NULL) || (p_BytesRead == NULL))
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
    else if((p_Socket->isDataReceived == false) || (Length == 0))
    {
        *p_BytesRead = 0;

        return SIM70XX_ERR_OK;
    }

    vTaskSuspend(p_Device.Internal.TaskHandle);
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CARECV(p_Socket->ID, Length);
    SIM70XX_UART_SendLine(p_Device.UART, Command->Command);
    delete Command;

    do
    {
        int c;

        c = SIM70XX_UART_Read(p_Device.UART);
        if(c != -1)
        {
            *Buffer = c;
            BytesRead++;
            Buffer++;
        }
    } while(SIM70XX_UART_Available(p_Device.UART) > 0);

    p_Socket->isDataReceived = false;

    vTaskResume(p_Device.Internal.TaskHandle);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_TCP_Client_Destroy(SIM7080_t& p_Device, SIM7080_TCP_Socket_t* p_Socket)
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
    else if(p_Socket->isCreated == false)
    {
        return SIM70XX_ERR_NOT_CREATED;
    }
    else if(p_Socket->isConnected == false)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CACLOSE(p_Socket->ID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    p_Socket->isConnected = false;
    p_Socket->isCreated = false;

    return SIM70XX_ERR_OK;
}

#endif