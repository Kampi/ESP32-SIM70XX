 /*
 * sim7080_udp_client_api.h
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

#ifndef SIM7080_UDP_CLIENT_API_H_
#define SIM7080_UDP_CLIENT_API_H_

#include "sim7080_defs.h"
#include "sim70xx_errors.h"
#include "sim7080_tcpip_defs.h"

#include <sdkconfig.h>

/** @brief          Check if data are received for a specific socket.
 *  @param p_Socket Pointer to TCP socket object
 *  @return         #true when data were received
 */
inline __attribute__((always_inline)) bool SIM7080_UDP_Client_isDataAvailable(SIM7080_TCPIP_Socket_t* p_Socket)
{
    if((p_Socket == NULL) || (p_Socket->Internal.Type != SIM7080_TCP_TYPE_UDP))
    {
        return false;
    }

    return p_Socket->Internal.isDataReceived;
}

/** @brief              Create a socket for a UDP client.
 *  @param p_Device     SIM7080 device object
 *  @param IP           IP address
 *  @param Port         UDP port
 *  @param p_Socket     Pointer to UDP socket object
 *  @param CID          (Optional) Context Identifier
 *  @param ReadManually (Optional) Set to #true to automatically read receiving data
 *                      NOTE: Not supported yet
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_UDP_Client_Create(SIM7080_t& p_Device, std::string IP, uint16_t Port, SIM7080_TCPIP_Socket_t* p_Socket, uint8_t CID = 0, bool ReadManually = false);

/** @brief          Open a UDP connection to a remote server.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to UDP socket object
 *  @param PDP      (Optional) PDP context that should be used
 *  @param p_Result (Optional) Pointer to UDP error code
 *                  NOTE: Can be used for error tracking
 *  @param Timeout  (Optional) Transmission timeout in milliseconds
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_UDP_Client_Connect(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket, uint8_t PDP = 0, SIM7080_TCP_Error_t* p_Result = NULL);

/** @brief              Transmit a UDP message.
 *  @param p_Device     SIM7080 device object
 *  @param p_Socket     Pointer to UDP socket object
 *  @param p_Buffer     Pointer to data buffer
 *  @param Length       Data length
 *  @param Retries      (Optional) Number of retries to transmit a packet
 *  @param Timeout      (Optional) Transmission timeout in milliseconds
 *  @param PacketSize   (Optional) Transmission size in bytes
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_UDP_Client_Transmit(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket, const void* p_Buffer, uint32_t Length, uint8_t Retries = 20, uint16_t Timeout = 1000, uint16_t PacketSize = SIM7080_TCP_MAX_PAYLOAD_SIZE);

/** @brief          Receive a UDP message.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to UDP socket object
 *  @param p_Buffer Pointer to data buffer
 *  @param Length   (Optional) Number of bytes to read from the buffer
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_UDP_Client_Receive(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket, std::string* p_Buffer, uint32_t Length = 1024);

/** @brief          Disconnect a UDP connection from a remote server.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to UDP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_UDP_Client_Disconnect(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket);

/** @brief          Close a UDP connection and release the socket.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to UDP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_UDP_Client_Destroy(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket);

#ifdef CONFIG_SIM70XX_DRIVER_WITH_SSL
    /** @brief          Enable / Disable SSL support for the UDP client.
     *  @param p_Device SIM7080 device object
     *  @param Enable   Enable / Disable SSL
     *  @param CID      (Optional) Context Identifier
     *  @return         SIM70XX_ERR_OK when successful
     */
    SIM70XX_Error_t SIM7080_UDP_Client_EnableSSL(SIM7080_t& p_Device, bool Enable, uint8_t CID = 0);
#endif

#endif /* SIM7080_UDP_CLIENT_API_H_ */