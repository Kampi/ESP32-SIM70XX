 /*
 * sim7080_ntp_defs.h
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

#ifndef SIM7080_NTP_DEFS_H_
#define SIM7080_NTP_DEFS_H_

#include <time.h>
#include <string>
#include <stdint.h>
#include <stdbool.h>

/** @brief SIM7080 NTP error definitions.
 */
typedef enum
{
    SIM7080_NTP_ERROR_OK                = 1,        /**< No error. */
    SIM7080_NTP_ERROR_NETWORK           = 61,       /**< Network error. */
    SIM7080_NTP_ERROR_DNS,                          /**< DNS resolution error. */
    SIM7080_NTP_ERROR_CONNECTION,                   /**< Connection error. */
    SIM7080_NTP_ERROR_SERVICE_RESP,                 /**< Service response error. */
    SIM7080_NTP_ERROR_SERVICE_TIME,                 /**< Service response timeout. */
} SIM7080_NTP_Error_t;

/** @brief SIM7080 NTP mode definitions.
 */
typedef enum
{
    SIM7080_NTP_MODE_SYNC               = 0,        /**< Just set UTC to localtime. */
    SIM7080_NTP_MODE_FETCH,                         /**< Just output UTC time to AT port. */
    SIM7080_NTP_MODE_BOTH,                          /**< Set UTC to localtime and output UTC time to AT port. */
} SIM7080_NTP_Mode_t;

#endif /* SIM7080_NTP_DEFS_H_ */