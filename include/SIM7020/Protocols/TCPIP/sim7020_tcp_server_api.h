 /*
 * sim7020_tcp_server_api.h
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

#ifndef SIM7020_TCP_SERVER_API_H_
#define SIM7020_TCP_SERVER_API_H_

#include "sim7020_defs.h"
#include "sim70xx_errors.h"
#include "sim7020_tcpip_defs.h"

/** @brief          Create a TCP server socket.
 *  @param p_Device SIM7020 device object
 *  @param IP       IP address
 *  @param Port     TCP port
 *  @param p_Socket Pointer to TCP/IP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCP_Server_Create(SIM7020_t& p_Device, std::string IP, uint16_t Port, SIM7020_TCPIP_Socket_t* p_Socket);

/** @brief          Bind the socket to the IP address and the port.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to TCP/IP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCP_Server_Bind(SIM7020_t& p_Device, SIM7020_TCPIP_Socket_t* p_Socket);

/** @brief          Open the socket and start listening.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to TCP/IP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCP_Server_Listen(SIM7020_t& p_Device, SIM7020_TCPIP_Socket_t* p_Socket);

/** @brief          Destroy a TCP server socket.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to TCP/IP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCP_Server_Destroy(SIM7020_t& p_Device, SIM7020_TCPIP_Socket_t* p_Socket);

#endif /* SIM7020_TCP_SERVER_API_H_ */