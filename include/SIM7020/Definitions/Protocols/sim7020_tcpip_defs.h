 /*
 * sim7020_tcpip_defs.h
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

#ifndef SIM7020_TCPIP_DEFS_H_
#define SIM7020_TCPIP_DEFS_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

/** @brief TCP connection error definitions.
 */
typedef enum
{
    SIM7020_TCP_ERROR_COMMON    = -1,               /**< Common error. */
    SIM7020_TCP_ERROR_ROUTE     = 1,                /**< Route error. */
    SIM7020_TCP_ERROR_ABORT,                        /**< Connection abort error. */
    SIM7020_TCP_ERROR_RESET,                        /**< Reset error. */
    SIM7020_TCP_ERROR_CONNECTED,                    /**< Connected error. */
    SIM7020_TCP_ERROR_VALUE,                        /**< Value error. */
    SIM7020_TCP_ERROR_BUFFER,                       /**< Buffer error. */
    SIM7020_TCP_ERROR_BLOCK,                        /**< Block error. */
    SIM7020_TCP_ERROR_ADDR,                         /**< Address in use error. */
    SIM7020_TCP_ERROR_ALR,                          /**< ALR connecting error. */
    SIM7020_TCP_ERROR_ALR_CONNECT,                  /**< ALR connected error. */
    SIM7020_TCP_ERROR_NETIF,                        /**< NETIF error. */
    SIM7020_TCP_ERROR_PARAMETER,                    /**< Parameter error. */
} SIM7020_TCP_Error_t;

/** @brief TCP socket domains.
 */
typedef enum
{
    SIM7020_TCP_DOMAIN_IPV4 = 1,                    /**< IPv4 socket. */
    SIM7020_TCP_DOMAIN_IPV6,                        /**< IPv4 socket. */
} SIM7020_TCP_Domain_t;

/** @brief TCP socket types.
 */
typedef enum
{
    SIM7020_TCP_TYPE_TCP    = 1,                    /**< TCP socket. */
    SIM7020_TCP_TYPE_UDP,                           /**< UDP socket. */
    SIM7020_TCP_TYPE_RAW,                           /**< RAW socket. */
} SIM7020_TCP_Type_t;

/** @brief TCP protocol types.
 */
typedef enum
{
    SIM7020_TCP_PROT_IP     = 1,                    /**< IP protocol. */
    SIM7020_TCP_PROT_ICMP,                          /**< ICMP protocol. */
} SIM7020_TCP_Protocol_t;

/** @brief DNS error codes.
 */
typedef enum
{
    SIM7020_DNS_ERR_NETWORK = 3,                    /**< Network error. */
    SIM7020_DNS_ERR_COMMON  = 8,                    /**< Common DNS error. */
} SIM7020_DNS_Err_t;

/** @brief SIM7020 Ping configuration object.
 */
typedef struct
{
    std::string IP;                                 /**< IP address of the remote host. */
    int Retries;                                    /**< The number of Ping Echo Request to send.
                                                         NOTE: -1 will use the default value (4). Minimum 1, Maximum 100. */
    int16_t DataLength;                             /**< The length of Ping Echo Request data.
                                                         NOTE: -1 will use the default value (32). Minimum 0, Maximum 5120. */
    int16_t Timeout;                                /**< The timeout, in units of 100ms, waiting for a single Echo Reply.
                                                         NOTE: -1 will use the default value (100). Minimum 0, Maximum 600. */
} SIM7020_Ping_t;

/** @brief SIM7020 Ping response object.
 */
typedef struct
{
    std::string IP;                                 /**< IP address of the remote host. */
    uint8_t ReplyTime;                              /**< Time, in units of 100ms, required to receive the response. */
    uint8_t TTL;                                    /**< Time to live. */
} SIM7020_PingRes_t;

/** @brief SIM7020 TCP/IP Socket object.
 */
typedef struct
{
    std::string IP;                                 /**< IP address. */
    uint16_t Port;                                  /**< Target port. */
    uint8_t CID;                                    /**< PDP context ID. */
    uint16_t Timeout;                               /**< TCP command timeout in seconds. */
    SIM7020_TCP_Domain_t Domain;                    /**< Socket IP address domain. */
    SIM7020_TCP_Protocol_t Protocol;                /**< Socket protocol .*/
    uint8_t ID;                                     /**< Socket ID from the module.
                                                         NOTE: Handled by the device driver. */
    bool isConnected;                               /**< Socket connected.
                                                         NOTE: Handled by the device driver. */
    bool isCreated;                                 /**< #true when the socket is created.
                                                         NOTE: Handled by the device driver. */
    SIM7020_TCP_Type_t Type;                        /**< Socket type.
                                                         NOTE: Handled by the device driver. */
} SIM7020_TCP_Socket_t;

#endif /* SIM7020_TCPIP_DEFS_H_ */