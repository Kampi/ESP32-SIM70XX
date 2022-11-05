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

#include "sim7020_tcp_client_api.h"
#include "sim7020_tcp_server_api.h"
#include "sim7020_udp_client_api.h"

/** @brief          Perform a ping.
 *  @param p_Device SIM7020 device object
 *  @param p_Config Pointer to SIM7020 ping configuration object
 *  @param p_Result Pointer to list with ping results
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_TCPIP_Ping(SIM7020_t& p_Device, const SIM7020_Ping_t* const p_Config, std::vector<SIM7020_PingRes_t>* p_Result);

#endif /* SIM7020_TCPIP_H_ */