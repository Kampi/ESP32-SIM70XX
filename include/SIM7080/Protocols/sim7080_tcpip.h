 /*
 * sim7080_tcpip.h
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

#ifndef SIM7080_TCPIP_H_
#define SIM7080_TCPIP_H_

#include "sim7080_defs.h"
#include "sim70xx_errors.h"
#include "sim7080_tcpip_defs.h"
/** @brief Maximum number of bytes for a single TCP / UDP transmission.
 */
#define SIM7080_TCP_MAX_PAYLOAD_SIZE                        1460

/** @brief          Perform a ping.
 *  @param p_Device SIM7080 device object
 *  @param p_Config Pointer to Ping configuration object
 *  @param p_Result Pointer to list with ping results
 *  @param IPv6     (Optional) Set to #true to perform an IPv6 ping
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_TCP_Ping(SIM7080_t& p_Device, SIM7080_Ping_t* p_Config, std::vector<SIM7080_PingRes_t>* p_Result, bool IPv6 = false);

/** @brief          Check if data are received for a specific socket.
 *  @param p_Socket Pointer to TCP socket object
 *  @return         #true when data are received
 */
bool inline __attribute__((always_inline)) SIM7080_TCP_Client_isDataAvailable(SIM7080_TCP_Socket_t* p_Socket)
{
    if(p_Socket == NULL)
    {
        return false;
    }

    return p_Socket->isDataReceived;
}

/** @brief              Create a socket for a TCP client.
 *  @param p_Device     SIM7080 device object
 *  @param IP           IP address
 *  @param Port         TCP port
 *  @param p_Socket     Pointer to TCP socket object
 *  @param CID          (Optional) Socket CID
 *  @param ReadManually (Optional) Set to #true to automatically read receiving data
 *                      NOTE: Not supported yet
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_TCP_Client_Create(SIM7080_t& p_Device, std::string IP, uint16_t Port, SIM7080_TCP_Socket_t* p_Socket, uint8_t CID = 0, bool ReadManually = false);

/** @brief          Open a TCP connection to a remote server.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to TCP socket object
 *  @param PDP      (Optional) PDP context that should be used
 *  @param p_Result (Optional) Pointer to TCP error code
 *                  NOTE: Can be used for error tracking
 *  @param Timeout  (Optional) Transmission timeout in milliseconds
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_TCP_Client_Connect(SIM7080_t& p_Device, SIM7080_TCP_Socket_t* p_Socket, uint8_t PDP = 0, SIM7080_TCP_Error_t* p_Result = NULL);

/** @brief              Transmit a TCP message.
 *  @param p_Device     SIM7080 device object
 *  @param p_Socket     Pointer to TCP socket object
 *  @param p_Buffer     Pointer to data buffer
 *  @param Length       Data length
 *  @param Timeout      (Optional) Transmission timeout in milliseconds
 *  @param PacketSize   (Optional) Message transmission packet size in bytes
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_TCP_Client_Transmit(SIM7080_t& p_Device, SIM7080_TCP_Socket_t* p_Socket, const void* p_Buffer, uint32_t Length, uint16_t Timeout = 1000, uint16_t PacketSize = SIM7080_TCP_MAX_PAYLOAD_SIZE);

/** @brief          Receive a TCP message.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to TCP socket object
 *  @param Length   Number of bytes to read from the buffer
 *  @param p_Buffer Pointer to data buffer
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_TCP_Client_Receive(SIM7080_t& p_Device, SIM7080_TCP_Socket_t* p_Socket, uint32_t Length, std::string* p_Buffer);

/** @brief          Close a TCP connection and release the socket.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to TCP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_TCP_Client_Destroy(SIM7080_t& p_Device, SIM7080_TCP_Socket_t* p_Socket);

#endif /* SIM7080_TCPIP_H_ */