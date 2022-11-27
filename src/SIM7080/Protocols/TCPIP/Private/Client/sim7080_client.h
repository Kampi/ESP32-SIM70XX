 /*
 * SIM7080_client.h
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

#ifndef SIM7080_CLIENT_H_
#define SIM7080_CLIENT_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

#include "SIM7080.h"
#include "SIM7080_tcpip.h"

/** @brief              Create a common TCP / UDP client socket.
 *  @param p_Device     SIM7080 device object
 *  @param Type         Socket type
 *  @param IP           IP address
 *  @param Port         TCP port
 *  @param p_Socket     Pointer to TCP socket object
 *  @param CID          (Optional) Context Identifier
 *  @param ReadManually (Optional) Set to #true to automatically read receiving data
 *                      NOTE: Not supported yet
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Client_CreateSocket(SIM7080_t& p_Device, SIM7080_TCP_Type_t Type, std::string IP, uint16_t Port, SIM7080_TCPIP_Socket_t* p_Socket, uint8_t CID, bool ReadManually);

/** @brief          Open a connection to a remote server.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to TCP socket object
 *  @param p_PDP    Pointer to PDP context
 *  @param p_Result (Optional) Pointer to TCP error code
 *                  NOTE: Can be used for error tracking
 *  @param Timeout  (Optional) Transmission timeout in milliseconds
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Client_ConnectSocket(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket, SIM7080_PDP_Context_t* p_PDP, SIM7080_TCP_Error_t* p_Result = NULL);

/** @brief              Transmit a TCP / UDP message.
 *  @param p_Device     SIM7080 device object
 *  @param p_Socket     Pointer to TCP socket object
 *  @param p_Buffer     Pointer to data buffer
 *  @param Length       Data length
 *  @param Retries      (Optional) Number of retries to transmit a packet
 *  @param Timeout      (Optional) Transmission timeout in milliseconds
 *  @param PacketSize   (Optional) Transmission size in bytes
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Client_Transmit(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket, const void* p_Buffer, uint32_t Length, uint8_t Retries = 20, uint16_t Timeout = 1000, uint16_t PacketSize = SIM7080_TCP_MAX_PAYLOAD_SIZE);

/** @brief          Receive a TCP /UDP message.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to TCP socket object
 *  @param p_Buffer Pointer to data buffer
 *  @param Length   (Optional) Number of bytes to read from the buffer
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Client_Receive(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket, std::string* p_Buffer, uint32_t Length = 1024);

/** @brief          Disconnect a common TCP / UDP connection from a remote server.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to TCPIP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Client_DisconnectSocket(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket);

/** @brief          Destroy a common TCPIP / UDP socket.
 *  @param p_Device SIM7080 device object
 *  @param p_Socket Pointer to TCPIP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Client_DestroySocket(SIM7080_t& p_Device, SIM7080_TCPIP_Socket_t* p_Socket);

#ifdef CONFIG_SIM70XX_DRIVER_WITH_SSL
    /** @brief          Enable / Disable SSL support for the TCP / DUP client.
     *  @param p_Device SIM7080 device object
     *  @param Enable   Enable / Disable SSL
     *  @param CID      (Optional) Context Identifier
     *  @return         SIM70XX_ERR_OK when successful
     */
    SIM70XX_Error_t SIM7080_Client_EnableSSL(SIM7080_t& p_Device, bool Enable, uint8_t CID = 0);
#endif

#endif /* SIM7080_CLIENT_H_ */