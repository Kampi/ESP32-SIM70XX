 /*
 * sim7080_http_defs.h
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

#ifndef SIM7080_HTTP_DEFS_H_
#define SIM7080_HTTP_DEFS_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

/** @brief SIM7080 HTTP error codes.
 */
typedef enum
{
    SIM7080_HTTP_ERR_DC         = -1,               /**< Device disconnected. */
    SIM7080_HTTP_ERR_CLOSED     = -2,               /**< Connection was closed by a remote host. */
    SIM7080_HTTP_ERR_UNKNOWN    = -3,               /**< An unknown error occurred. */
    SIM7080_HTTP_ERR_PROTOCOL   = -4,               /**< A protocol error occurred. */
    SIM7080_HTTP_ERR_HOSTNAME   = -5,               /**< Could not resolve the hostname. */
    SIM7080_HTTP_ERR_URL        = -6,               /**< A URL parse error occurred. */
} SIM7080_HTTP_Error_t;

/** @brief SIM7080 HTTP methods.
 */
typedef enum
{
    SIM7080_HTTP_GET            = 0,                /**< HTTP GET method. */
    SIM7080_HTTP_POST,                              /**< HTTP POST method. */
    SIM7080_HTTP_PUT,                               /**< HTTP PUT method. */
    SIM7080_HTTP_DELETE,                            /**< HTTP DELETE method. */
} SIM7080_HTTP_Method_t;

/** @brief SIM7020 HTTP Socket object.
 */
typedef struct
{
    std::string Host;                               /**< HTTP(S) Host URL. */
    uint16_t Timeout;                               /**< Socket timeout in seconds. */
    uint8_t ID;                                     /**< Socket ID from the module.
                                                         NOTE: Handled by the device driver. */
    bool isConnected;                               /**< #true when the socket is connected.
                                                         NOTE: Handled by the device driver. */
    bool isCreated;                                 /**< #true when the socket is created.
                                                         NOTE: Handled by the device driver. */
    SIM7080_HTTP_Error_t Error;                     /**< Socket error.
                                                         NOTE: Handled by the device driver. */
} SIM7080_HTTP_Socket_t;

#endif /* SIM7080_HTTP_DEFS_H_ */