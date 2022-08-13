 /*
 * sim7080_pdp_defs.h
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

#ifndef SIM7080_PDP_DEFS_H_
#define SIM7080_PDP_DEFS_H_

#include <stdint.h>
#include <stdbool.h>

/** @brief SIM7080 supported PDP actions.
 */
typedef enum
{
    SIM7080_PDP_DISABLE     = 0,                    /**< Deactive. */
    SIM7080_PDP_ENABLE,                             /**< Active. */
    SIM7080_PDP_AUTO,                               /**< Deactive. */
} SIM7080_PDP_Action_t;

/** @brief SIM7080 supported network states.
 */
typedef enum
{
    SIM7080_PDP_STATE_INACTIVE = 0,                 /**< Deactive. */
    SIM7080_PDP_STATE_ACTIVE,                       /**< Active. */
    SIM7080_PDP_STATE_OPERATIOn,                    /**< In operation. */
} SIM7080_PDP_Status_t;

/** @brief SIM7080 supported GPRS PDP types.
 */
typedef enum
{
    SIM7080_PDP_GPRS_IP     = 0,                    /**< Internet Protocol (IETF STD 5). */
    SIM7080_PDP_GPRS_IPV6,                          /**< Internet Protocol, version 6 (IETF RFC 2460). */
    SIM7080_PDP_GPRS_IPV4V6,                        /**< Virtual <PDP_type) introduced to handle dual IP stack UE capability(see 3GPP TS 24.301). */
    SIM7080_PDP_GPRS_NO_IP,                         /**< Transfer of Non-IP data to external packet data Network (see 3GPP TS 24.301). */
} SIM7080_PDP_GPRS_Type_t;

/** @brief SIM7080 supported IP PDP types.
 */
typedef enum
{
    SIM7080_PDP_IP_IP       = 0,                    /**< Dual PDN Stack. */
    SIM7080_PDP_IP_IPV4,                            /**< Internet Protocol, version 4. */
    SIM7080_PDP_IP_IPV6,                            /**< Internet Protocol, version 6. */
    SIM7080_PDP_IP_NONIP,                           /**< Non IP. */
    SIM7080_PDP_IP_EX_NONIP,                        /**< */
} SIM7080_PDP_IP_Type_t;

/** @brief SIM7080 supported IP PDP authentication methods.
 */
typedef enum
{
    SIM7080_PDP_IP_AUTH_NONE    = 0,                /**< */
    SIM7080_PDP_IP_AUTH_PAP,                        /**< */
    SIM7080_PDP_IP_AUTH_CHAP,                       /**< */
    SIM7080_PDP_AUTH_BOTH,                          /**< */
} SIM7080_PDP_IP_Auth_t;

/** @brief SIM7080 network status object definition.
 */
typedef struct
{
    uint8_t ID;                                     /**< PDP Context Identifier. */
    SIM7080_PDP_Status_t Status;                    /**< Network status. */
    std::string IP;                                 /**< IP address. */
} SIM7080_PDP_Network_t;

#endif /* SIM7080_PDP_DEFS_H_ */