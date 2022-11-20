 /*
 * sim7080_tcp_client.cpp
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
#include "sim7080_tcpip.h"
#include "Private/Client/sim7080_client.h"

SIM70XX_Error_t SIM7080_TCP_Client_Create(SIM7080_t& p_Device, std::string IP, uint16_t Port, SIM7080_TCPIP_Socket_t* p_Socket, uint8_t CID, bool ReadManually)
{
    return SIM7080_Client_CreateSocket(p_Device, SIM7080_TCP_TYPE_TCP, IP, Port, p_Socket, CID, ReadManually);
}

SIM70XX_Error_t SIM7080_TCP_Client_Connect(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket, uint8_t PDP, SIM7080_TCP_Error_t* p_Result)
{
    if(p_Socket->Internal.Type != SIM7080_TCP_TYPE_TCP)
    {
        return SIM70XX_ERR_INVALID_SOCKET;
    }

    return SIM7080_Client_ConnectSocket(p_Device, p_Socket, PDP, p_Result);
}

SIM70XX_Error_t SIM7080_TCP_Client_Transmit(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket, const void* p_Buffer, uint32_t Length, uint8_t Retries, uint16_t Timeout, uint16_t PacketSize)
{
    if(p_Socket->Internal.Type != SIM7080_TCP_TYPE_TCP)
    {
        return SIM70XX_ERR_INVALID_SOCKET;
    }

    return SIM7080_Client_Transmit(p_Device, p_Socket, p_Buffer, Length, Retries, Timeout, PacketSize);
}

SIM70XX_Error_t SIM7080_TCP_Client_Receive(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket, std::string* p_Buffer, uint32_t Length)
{
    if(p_Socket->Internal.Type != SIM7080_TCP_TYPE_TCP)
    {
        return SIM70XX_ERR_INVALID_SOCKET;
    }

    return SIM7080_Client_Receive(p_Device, p_Socket, p_Buffer, Length);
}

SIM70XX_Error_t SIM7080_TCP_Client_Disconnect(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket)
{
    if(p_Socket->Internal.Type != SIM7080_TCP_TYPE_TCP)
    {
        return SIM70XX_ERR_INVALID_SOCKET;
    }

    return SIM7080_Client_DisconnectSocket(p_Device, p_Socket);
}

SIM70XX_Error_t SIM7080_TCP_Client_Destroy(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket)
{
    if(p_Socket->Internal.Type != SIM7080_TCP_TYPE_TCP)
    {
        return SIM70XX_ERR_INVALID_SOCKET;
    }

    return SIM7080_Client_DestroySocket(p_Device, p_Socket);
}

#ifdef CONFIG_SIM70XX_DRIVER_WITH_SSL
    SIM70XX_Error_t SIM7080_TCP_Client_EnableSSL(SIM7080_t& p_Device, bool Enable, uint8_t CID)
    {
        if(p_Socket->Internal.Type != SIM7080_TCP_TYPE_TCP)
        {
            return SIM70XX_ERR_INVALID_SOCKET;
        }

        return SIM7080_Client_EnableSSL(p_Device, Enable, CID);
    }
#endif

#endif