 /*
 * sim7080_tcpip_defs.h
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

#ifndef SIM7080_TCPIP_DEFS_H_
#define SIM7080_TCPIP_DEFS_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

/** @brief SIM7080 ping configuration object.
 */
typedef struct
{
    std::string IP;                                 /**< IP address of the remote host. */
    int16_t Retries;                                /**< The number of Ping Echo Request to send.
                                                         NOTE: -1 will use the default value (4). Minimum 1, Maximum 500. */
    int16_t Size;                                   /**< Number of data bytes to send, range: 1~1400.
                                                         NOTE: -1 will use the default value (32). Minimum 1, Maximum 1400. */
    int32_t Timeout;                                /**< The timeout, in units of 100ms, waiting for a single Echo Reply.
                                                         NOTE: -1 will use the default value (100). Minimum 0, Maximum 60000. */
} SIM7080_Ping_t;

/** @brief SIM7080 Ping response object.
 */
typedef struct
{
    std::string IP;                                 /**< IP address of the remote host. */
    uint16_t ReplyTime;                             /**< Time, in units of 100ms, required to receive the response. */
} SIM7080_PingRes_t;

#endif /* SIM7080_TCPIP_DEFS_H_ */