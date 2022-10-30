 /*
 * sim7020_udp_client.cpp
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

#if((CONFIG_SIMXX_DEV == 7020) && (defined CONFIG_SIM70XX_DRIVER_WITH_TCPIP))

#include <esp_log.h>

#include "sim7020.h"
#include "sim7020_tcpip.h"
#include "Private/sim7020_client.h"
#include "../../../Private/Queue/sim70xx_queue.h"
#include "../../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7020_UDP_Client";

SIM70XX_Error_t SIM7020_UDP_Client_Create(SIM7020_t& p_Device, std::string IP, uint16_t Port, SIM7020_TCPIP_Socket_t* p_Socket, uint16_t Timeout, uint8_t CID, SIM7020_TCP_Domain_t Domain, SIM7020_TCP_Protocol_t Protocol)
{
    return SIM7020_Client_CreateSocket(p_Device, SIM7020_TCP_TYPE_UDP, IP, Port, p_Socket, Timeout, CID, Domain, Protocol);
}

SIM70XX_Error_t SIM7020_UDP_Client_Connect(SIM7020_t& p_Device, SIM7020_TCPIP_Socket_t* p_Socket)
{
    if(p_Socket->Internal.Type != SIM7020_TCP_TYPE_UDP)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    return SIM7020_Client_ConnectSocket(p_Device, p_Socket);
}

SIM70XX_Error_t SIM7020_UDP_Client_Transmit(SIM7020_t& p_Device, SIM7020_TCPIP_Socket_t* p_Socket, const void* p_Buffer, uint32_t Length, uint16_t PacketSize)
{
    if(p_Socket->Internal.Type != SIM7020_TCP_TYPE_UDP)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    return SIM7020_Client_Transmit(p_Device, p_Socket, p_Buffer, Length, PacketSize);
}

SIM70XX_Error_t SIM7020_UDP_Client_Receive(SIM7020_t& p_Device, SIM7020_TCPIP_Socket_t* p_Socket, std::string* p_Buffer)
{
    if(p_Socket->Internal.Type != SIM7020_TCP_TYPE_UDP)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    return SIM7020_Client_Receive(p_Device, p_Socket, p_Buffer);
}

SIM70XX_Error_t SIM7020_UDP_Client_Disconnect(SIM7020_t& p_Device, SIM7020_TCPIP_Socket_t* p_Socket)
{
    if(p_Socket->Internal.Type != SIM7020_TCP_TYPE_UDP)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    return SIM7020_Client_DisconnectSocket(p_Device, p_Socket);
}

SIM70XX_Error_t SIM7020_UDP_Client_Destroy(SIM7020_t& p_Device, SIM7020_TCPIP_Socket_t* p_Socket)
{
    if(p_Socket->Internal.Type != SIM7020_TCP_TYPE_UDP)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    return SIM7020_Client_DestroySocket(p_Device, p_Socket);
}

#endif