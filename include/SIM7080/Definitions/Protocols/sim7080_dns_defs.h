 /*
 * sim7080_dns_defs.h
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

#ifndef SIM7080_DNS_DEFS_H_
#define SIM7080_DNS_DEFS_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

/** @brief SIM7080 DNS error definitions.
 */
typedef enum
{
    SIM7080_DNS_ERROR_OK        = -1,               /**< Common error. */
    SIM7080_DNS_ERROR_NOT_AUTH,                     /**< Not authenticated error. */
    SIM7080_DNS_ERROR_INVALID_PARA,                 /**< Invalid parameter error. */
    SIM7080_DNS_ERROR_NETWORK,                      /**< No network error. */
    SIM7080_DNS_ERROR_NO_SERVER,                    /**< No server found error. */
    SIM7080_DNS_ERROR_TIMEOUT,                      /**< Timeout error. */
    SIM7080_DNS_ERROR_NO_CONFIG,                    /**< No configuration error. */
    SIM7080_DNS_ERROR_NO_MEMORY,                    /**< No memory error. */
    SIM7080_DNS_ERROR_UNKNOWN,                      /**< Unknown error. */
} SIM7080_DNS_Error_t;

#endif /* SIM7080_DNS_DEFS_H_ */