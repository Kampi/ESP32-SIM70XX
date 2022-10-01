 /*
 * sim7020_tcpip.h
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

#ifndef SIM7020_TCPIP_H_
#define SIM7020_TCPIP_H_

#include "sim7020_defs.h"
#include "sim70xx_errors.h"
#include "sim7020_tcpip_defs.h"

/** @brief Maximum number of bytes for a single TCP / UDP transmission.
 */
#define SIM7020_TCP_MAX_PAYLOAD_SIZE                        512

/** @brief          Perform a ping.
 *  @param p_Device SIM7020 device object
 *  @param p_Config Pointer to SIM7020 ping configuration object
 *  @param p_Result Pointer to list with Ping results
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCP_Ping(SIM7020_t& p_Device, SIM7020_Ping_t* p_Config, std::vector<SIM7020_PingRes_t>* p_Result);

/** @brief          Check if data are received for a specific socket.
 *  @param p_Socket Pointer to TCP socket object
 *  @return         #true when data are received
 */
bool inline __attribute__((always_inline)) SIM7020_TCP_Client_isDataAvailable(SIM7020_TCP_Socket_t* p_Socket)
{
    if(p_Socket == NULL)
    {
        return false;
    }

    return p_Socket->Internal.isDataReceived;
}

/** @brief          Create a TCP socket.
 *  @param p_Device SIM7020 device object
 *  @param IP       IP address
 *  @param Port     TCP port
 *  @param p_Socket Pointer to TCP socket object
 *  @param Timeout  (Optional) Timeout in seconds
 *  @param CID      (Optional) Context Identifier
 *  @param Domain   (Optional) Socket IP domain
 *  @param Protocol (Optional) Socket protocol
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCP_Client_Create(SIM7020_t& p_Device, std::string IP, uint16_t Port, SIM7020_TCP_Socket_t* p_Socket, uint16_t Timeout = 60, uint8_t CID = 1, SIM7020_TCP_Domain_t Domain = SIM7020_TCP_DOMAIN_IPV4, SIM7020_TCP_Protocol_t Protocol = SIM7020_TCP_PROT_IP);

/** @brief          Open a TCP connection to a remote server.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to TCP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCP_Client_Connect(SIM7020_t& p_Device, SIM7020_TCP_Socket_t* p_Socket);

/** @brief          Transmit a TCP message with a length up to 512 bytes.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to TCP socket object
 *  @param p_Buffer Pointer to data buffer
 *  @param Length   Data length
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCP_Client_Transmit(SIM7020_t& p_Device, SIM7020_TCP_Socket_t* p_Socket, const void* p_Buffer, uint32_t Length);

/** @brief          Receive a TCP message.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to TCP socket object
 *  @param p_Buffer Pointer to data buffer
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCP_Client_Receive(SIM7020_t& p_Device, SIM7020_TCP_Socket_t* p_Socket, std::string* p_Buffer);

/** @brief          Close a TCP connection and release the socket.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to TCP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCP_Client_Destroy(SIM7020_t& p_Device, SIM7020_TCP_Socket_t* p_Socket);

#endif /* SIM7020_TCPIP_H_ */