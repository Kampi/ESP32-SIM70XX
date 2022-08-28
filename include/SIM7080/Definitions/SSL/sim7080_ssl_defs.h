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

/** @brief SIM7080 SSL type definitions.
 */
typedef enum
{
	SIM7080_SSL_PSK_WITH_RC4_128_SHA                        = 0x008A,
	SIM7080_SSL_PSK_WITH_3DES_EDE_CBC_SHA                   = 0x008B,
	SIM7080_SSL_PSK_WITH_AES_128_CBC_SHA                    = 0x008C,
	SIM7080_SSL_PSK_WITH_AES_256_CBC_SHA                    = 0x008D,
	SIM7080_SSL_PSK_WITH_AES_128_GCM_SHA256                 = 0x00A8,
	SIM7080_SSL_PSK_WITH_AES_256_GCM_SHA384                 = 0x00A9,
	SIM7080_SSL_PSK_WITH_AES_128_CBC_SHA256                 = 0x00AE,
	SIM7080_SSL_PSK_WITH_AES_256_CBC_SHA384                 = 0x00AF,
	SIM7080_SSL_PSK_WITH_AES_128_CCM_8                      = 0xC0A8,
	SIM7080_SSL_RSA_WITH_AES_128_CBC_SHA                    = 0x002F,
	SIM7080_SSL_DHE_RSA_WITH_AES_128_CBC_SHA                = 0x0033,
	SIM7080_SSL_RSA_WITH_AES_256_CBC_SHA                    = 0x0035,
	SIM7080_SSL_DHE_RSA_WITH_AES_256_CBC_SHA                = 0x0039,
	SIM7080_SSL_RSA_WITH_AES_128_CBC_SHA256                 = 0x003C,
	SIM7080_SSL_RSA_WITH_AES_256_CBC_SHA256                 = 0x003D,
	SIM7080_SSL_DHE_RSA_WITH_AES_128_CBC_SHA256             = 0x0067,
	SIM7080_SSL_DHE_RSA_WITH_AES_256_CBC_SHA256             = 0x006B,
	SIM7080_SSL_RSA_WITH_AES_128_GCM_SHA256                 = 0x009C,
	SIM7080_SSL_RSA_WITH_AES_256_GCM_SHA384                 = 0x009D,
	SIM7080_SSL_DHE_RSA_WITH_AES_128_GCM_SHA256             = 0x009E,
	SIM7080_SSL_DHE_RSA_WITH_AES_256_GCM_SHA384             = 0x009F,
	SIM7080_SSL_ECDH_ECDSA_WITH_AES_128_CBC_SHA             = 0xC004,
	SIM7080_SSL_ECDH_ECDSA_WITH_AES_256_CBC_SHA             = 0xC005,
	SIM7080_SSL_ECDHE_ECDSA_WITH_AES_128_CBC_SHA            = 0xC009,
	SIM7080_SSL_ECDHE_ECDSA_WITH_AES_256_CBC_SHA            = 0xC00A,
	SIM7080_SSL_ECDH_RSA_WITH_AES_128_CBC_SHA               = 0xC00E,
	SIM7080_SSL_ECDH_RSA_WITH_AES_256_CBC_SHA               = 0xC00F,
	SIM7080_SSL_ECDHE_RSA_WITH_AES_128_CBC_SHA              = 0xC013,
	SIM7080_SSL_ECDHE_RSA_WITH_AES_256_CBC_SHA              = 0xC014,
	SIM7080_SSL_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256         = 0xC023,
	SIM7080_SSL_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384         = 0xC024,
	SIM7080_SSL_ECDH_ECDSA_WITH_AES_128_CBC_SHA256          = 0xC025,
	SIM7080_SSL_ECDH_ECDSA_WITH_AES_256_CBC_SHA384          = 0xC026,
	SIM7080_SSL_ECDHE_RSA_WITH_AES_128_CBC_SHA256           = 0xC027,
	SIM7080_SSL_ECDHE_RSA_WITH_AES_256_CBC_SHA384           = 0xC028,
	SIM7080_SSL_ECDH_RSA_WITH_AES_128_CBC_SHA256            = 0xC029,
	SIM7080_SSL_ECDH_RSA_WITH_AES_256_CBC_SHA384            = 0xC02A,
	SIM7080_SSL_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256         = 0xC02B,
	SIM7080_SSL_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384         = 0xC02C,
	SIM7080_SSL_ECDH_ECDSA_WITH_AES_128_GCM_SHA256          = 0xC02D,
	SIM7080_SSL_ECDH_ECDSA_WITH_AES_256_GCM_SHA384          = 0xC02E,
	SIM7080_SSL_ECDHE_RSA_WITH_AES_128_GCM_SHA256           = 0xC02F,
	SIM7080_SSL_ECDHE_RSA_WITH_AES_256_GCM_SHA384           = 0xC030,
	SIM7080_SSL_ECDH_RSA_WITH_AES_128_GCM_SHA256            = 0xC031,
	SIM7080_SSL_ECDH_RSA_WITH_AES_256_GCM_SHA384            = 0xC032,
	SIM7080_SSL_ECDHE_ECDSA_WITH_AES_128_CCM_8              = 0xC0AE,
	SIM7080_SSL_RSA_WITH_AES_128_CCM                        = 0xC09C,
	SIM7080_SSL_RSA_WITH_AES_256_CCM                        = 0xC09D,
	SIM7080_SSL_DHE_RSA_WITH_AES_128_CCM                    = 0xC09E,
	SIM7080_SSL_DHE_RSA_WITH_AES_256_CCM                    = 0xC09F,
	SIM7080_SSL_RSA_WITH_AES_128_CCM_8                      = 0xC0A0,
	SIM7080_SSL_RSA_WITH_AES_256_CCM_8                      = 0xC0A1,
	SIM7080_SSL_DHE_RSA_WITH_AES_128_CCM_8                  = 0xC0A2,
	SIM7080_SSL_DHE_RSA_WITH_AES_256_CCM_8                  = 0xC0A3,
	SIM7080_SSL_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256     = 0xCC13,
	SIM7080_SSL_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256   = 0xCC14,
	SIM7080_SSL_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256       = 0xCC15,
} SIM7080_SSL_CipherSuite_t;

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
    uint8_t CipherIndex;                                    /**< */
    SIM7080_SSL_CipherSuite_t CiperSuite;                   /**< */
    bool IgnoreTime;                                        /**< */
} SIM7080_SSL_Config_t;

#endif /* SIM7080_SSL_DEFS_H_ */