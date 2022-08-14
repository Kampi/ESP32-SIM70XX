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

/** @brief SIM7080 TCP socket types definitions.
 */
typedef enum
{
    SIM7080_TCP_TYPE_TCP    = 1,                    /**< TCP socket. */
    SIM7080_TCP_TYPE_UDP,                           /**< UDP socket. */
    SIM7080_TCP_TYPE_NONIP,                         /**< NONIP socket. */
} SIM7080_TCP_Type_t;

/** @brief SIM7080 TCP error definitions.
 */
typedef enum
{
    SIM7080_TCP_ERROR_OK                = 0,        /**< No error. */
    SIM7080_TCP_ERROR_SOCK_ERR,                     /**< Socket error. */
    SIM7080_TCP_ERROR_NO_MEM,                       /**< No memory error. */
    SIM7080_TCP_ERROR_CON_LIMIT,                    /**< Connection limit reached. */
    SIM7080_TCP_ERROR_INVALID_PARAM,                /**< Invalid parameter. */
    SIM7080_TCP_ERROR_INVALID_IP,                   /**< Invalid IP address. */
    SIM7080_TCP_ERROR_NOT_SUPP,                     /**< Not supported. */
    SIM7080_TCP_ERROR_BIND_PORT         = 12,       /**< Can´t bind the port. */
    SIM7080_TCP_ERROR_LISTEN_PORT,                  /**< Can´t listen the port. */
    SIM7080_TCP_ERROR_RESOLVE_HOST      = 20,       /**< Can´t resolve the host. */
    SIM7080_TCP_ERROR_NOT_ACTIVE,                   /**< Network not active. */
    SIM7080_TCP_ERROR_REMOTE_REFUSE     = 23,       /**< Remote refuse. */
    SIM7080_TCP_ERROR_CERT_TIME,                    /**< Certificate´s time expired. */
    SIM7080_TCP_ERROR_CERT_NAME,                    /**< Certificate´s common name does not match. */
    SIM7080_TCP_ERROR_CERT_BOTH,                    /**< Certificate´s common name does not match and time expored. */
    SIM7080_TCP_ERROR_CONN_FAIL,                    /**< Connection failed. */
} SIM7080_TCP_Error_t;

/** @brief SIM7080 ping configuration object.
 */
typedef struct
{
    std::string Host;                               /**< Name or IP address of the remote host. */
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

/** @brief SIM7080 TCP/IP Socket object.
 */
typedef struct
{
    std::string IP;                                 /**< IP address. */
    uint16_t Port;                                  /**< Target port. */
    uint8_t ID;                                     /**< Socket ID from the module.
                                                         NOTE: Handled by the device driver. */
    bool isConnected;                               /**< Socket connected.
                                                         NOTE: Handled by the device driver. */
    bool isCreated;                                 /**< #true when the socket is created.
                                                         NOTE: Handled by the device driver. */
    bool isReadManually;                            /**< #true when the received data can only be read manually.
                                                         NOTE: Handled by the device driver. */
    bool isDataReceived;                            /**< Set to #true when data are received.
                                                         NOTE: Managed by the device driver. */
    SIM7080_TCP_Type_t Type;                        /**< Socket type.
                                                         NOTE: Handled by the device driver. */
} SIM7080_TCP_Socket_t;

#endif /* SIM7080_TCPIP_DEFS_H_ */