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

#include <sdkconfig.h>

/** @brief Maximum number of bytes for a single TCP / UDP transmission.
 */
#define SIM7080_TCP_MAX_PAYLOAD_SIZE                        1460

#include "sim7080_tcp_client_api.h"
#include "sim7080_udp_client_api.h"

/** @brief          Perform a ping.
 *  @param p_Device SIM7080 device object
 *  @param p_Config Pointer to Ping configuration object
 *  @param p_Result Pointer to list with ping results
 *  @param IPv6     (Optional) Set to #true to perform an IPv6 ping
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_TCPIP_Ping(SIM7080_t& p_Device, const SIM7080_Ping_t* const p_Config, std::vector<SIM7080_PingRes_t>* p_Result, bool IPv6 = false);

#endif /* SIM7080_TCPIP_H_ */