 /*
 * sim7020_tcp_client_api.h
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

#ifndef SIM7020_TCP_CLIENT_API_H_
#define SIM7020_TCP_CLIENT_API_H_

#include "sim7020_defs.h"
#include "sim70xx_errors.h"
#include "sim7020_tcpip_defs.h"

/** @brief          Check if data are received for a specific TCP socket.
 *  @param p_Socket Pointer to TCP/IP socket object
 *  @return         #true when data were received
 */
inline __attribute__((always_inline)) bool SIM7020_TCP_Client_isDataAvailable(SIM7020_TCPIP_Socket_t* p_Socket)
{
    if((p_Socket == NULL) || (p_Socket->Internal.Type != SIM7020_TCP_TYPE_TCP) || (p_Socket->Internal.isServer == true))
    {
        return false;
    }

    return p_Socket->Internal.isDataReceived;
}

/** @brief          Create a TCP client socket.
 *  @param p_Device SIM7020 device object
 *  @param IP       IP address
 *  @param Port     TCP port
 *  @param p_Socket Pointer to TCP/IP socket object
 *  @param Timeout  (Optional) Timeout in seconds
 *  @param CID      (Optional) Context Identifier
 *  @param Domain   (Optional) Socket IP domain
 *  @param Protocol (Optional) Socket protocol
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCP_Client_Create(SIM7020_t& p_Device, std::string IP, uint16_t Port, SIM7020_TCPIP_Socket_t* p_Socket, uint16_t Timeout = 60, uint8_t CID = 1, SIM7020_TCP_Domain_t Domain = SIM7020_TCP_DOMAIN_IPV4, SIM7020_TCP_Protocol_t Protocol = SIM7020_TCP_PROT_IP);

/** @brief          Open a TCP connection to a remote server.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to TCP/IP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCP_Client_Connect(SIM7020_t& p_Device, SIM7020_TCPIP_Socket_t* p_Socket);

/** @brief              Transmit a TCP message.
 *  @param p_Device     SIM7020 device object
 *  @param p_Socket     Pointer to TCP/IP socket object
 *  @param p_Buffer     Pointer to data buffer
 *  @param Length       Data length
 *  @param PacketSize   (Optional) Transmission packet size
 *                      NOTE: Value must be in between 1 and 512!
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCP_Client_Transmit(SIM7020_t& p_Device, SIM7020_TCPIP_Socket_t* p_Socket, const void* p_Buffer, uint32_t Length, uint16_t PacketSize = SIM7020_TCP_MAX_PAYLOAD_SIZE);

/** @brief          Receive a TCP message.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to TCP/IP socket object
 *  @param p_Buffer Pointer to data buffer
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCP_Client_Receive(SIM7020_t& p_Device, SIM7020_TCPIP_Socket_t* p_Socket, std::string* p_Buffer);

/** @brief          Disconnect a TCP connection from a remote server.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to TCP/IP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCP_Client_Disconnect(SIM7020_t& p_Device, SIM7020_TCPIP_Socket_t* p_Socket);

/** @brief          Destroy a TCP client socket.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to TCP/IP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCP_Client_Destroy(SIM7020_t& p_Device, SIM7020_TCPIP_Socket_t* p_Socket);

#endif /* SIM7020_TCP_CLIENT_API_H_ */