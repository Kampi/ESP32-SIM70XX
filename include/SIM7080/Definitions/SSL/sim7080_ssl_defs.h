 /*
 * sim7080_ssl_defs.h
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

#ifndef SIM7080_SSL_DEFS_H_
#define SIM7080_SSL_DEFS_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

/** @brief SIM7080 SSL version definitions.
 */
typedef enum
{
    SIM7080_SSL_VER_UNKNOWN = 0,                            /**< QAPI_NET_SSL_PROTOCOL_UNKNOWN. */
    SIM7080_SSL_VER_TLS_10,                                 /**< QAPI_NET_SSL_PROTOCOL_TLS_1_0. */
    SIM7080_SSL_VER_TLS_11,                                 /**< QAPI_NET_SSL_PROTOCOL_TLS_1_1. */
    SIM7080_SSL_VER_TLS_12,                                 /**< QAPI_NET_SSL_PROTOCOL_TLS_1_2. */
    SIM7080_SSL_VER_DTLS_10,                                /**< QAPI_NET_SSL_PROTOCOL_DTLS_1_0. */
    SIM7080_SSL_VER_DTLS_12                                 /**< QAPI_NET_SSL_PROTOCOL_DTLS_1_2. */
} SIM7080_SSL_Version_t;

/** @brief SIM7080 SSL type definitions.
 */
typedef enum
{
    SIM7080_SSL_TYPE_CERT   = 0,                            /**< QAPI_NET_SSL_CERTIFICATE_E. */
    SIM7080_SSL_TYPE_LIST,                                  /**< QAPI_NET_SSL_CA_LIST_E. */
    SIM7080_SSL_TYPE_TABLE,                                 /**< QAPI_NET_SSL_PSK_TABLE_E. */
} SIM7080_SSL_Type_t;

/** @brief SIM7080 SSL file object definition.
 */
typedef struct
{
    std::string Name;                                       /**< File name. */
    const uint8_t* p_Data;                                  /**< Pointer to file data. */
    uint16_t Size;                                          /**< File size. */
} SIM7080_SSL_File_t;

/** @brief SIM7080 SSL configuration object definition.
 */
typedef struct
{
    SIM7080_SSL_Version_t Version;                          /**< SSL version. */
} SIM7080_SSL_Config_t;

#endif /* SIM7080_SSL_DEFS_H_ */