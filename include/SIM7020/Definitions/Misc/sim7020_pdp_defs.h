 /*
 * sim7020_pdp_defs.h
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

#ifndef SIM7020_PDP_DEFS_H_
#define SIM7020_PDP_DEFS_H_

#include <stdint.h>
#include <stdbool.h>

/** @brief Supported PDP types.
 */
typedef enum
{
    SIM7020_PDP_IP          = 0,                    /**< Internet Protocol (IETF STD 5). */
    SIM7020_PDP_IPV6,                               /**< Internet Protocol, version 6 (IETF RFC 2460). */
    SIM7020_PDP_IPV4V6,                             /**< Virtual <PDP_type) introduced to handle dual IP stack UE capability(see 3GPP TS 24.301). */
    SIM7020_PDP_NO_IP,                              /**< Transfer of Non-IP data to external packet data Network (see 3GPP TS 24.301). */
} SIM7020_PDP_Type_t;

/** @brief PDP data compression definitions.
 */
typedef enum
{
    SIM7020_PDP_DCOMP_OFF   = 0,                    /**< Compression disabled. */
    SIM7020_PDP_DCOMP_ON,                           /**< Compression enabled. */
    SIM7020_PDP_DCOMP_V42,                          /**< Compression V.42bis. */
} SIM7020_PDP_DComp_t;

/** @brief PDP header compression definitions.
 */
typedef enum
{
    SIM7020_PDP_HCOMP_OFF   = 0,                    /**< Compression disabled. */
    SIM7020_PDP_HCOMP_ON,                           /**< Compression enabled. */
    SIM7020_PDP_HCOMP_1144,                         /**< RFC1144 (applicable for SNDCP only). */
    SIM7020_PDP_HCOMP_2507,                         /**< RFC 2507. */
    SIM7020_PDP_HCOMP_3095,                         /**< RFC 3095 (ROHC) (applicable for PDCP only). */
} SIM7020_PDP_HComp_t;

/** @brief SIM7020 PDP Context object definition.
 */
typedef struct
{
    uint8_t CID;                                    /**< PDP Context ID. */
    SIM7020_PDP_Type_t Type;                        /**< PDP Context type. */
    std::string APN;                                /**< Access Point Name. */
    std::string PDP_Address;                        /**< */
    SIM7020_PDP_DComp_t D_Comp;                     /**< PDP data compression. */
    SIM7020_PDP_HComp_t H_Comp;                     /**< PDP header compression. */
    uint8_t IPv4_MTU;                               /**< Influences how the MT/TA requests to get the IPv4 MTU size, see 3GPP TS 24.008 sub-clause 10.5.6.3.
                                                            0 Preference of IPv4 MTU size discovery not influenced by +CGDCONT
                                                            1 Preference of IPv4 MTU size discovery through NAS signaling. */
    uint8_t Non_MTU;                                /**< Influences how the MT/TA requests to get the Non-IP MTU size, see 3GPP TS 24.008 sub-clause 10.5.6.3.
                                                            0 Preference of Non-IP MTU size discovery not influenced by +CGDCONT
                                                            1 Preference of Non-IP MTU size discovery through NAS signaling. */
} SIM7020_PDP_Context_t;

/** @brief SIM7020 PDP Context status object definition.
 */
typedef struct
{
    uint8_t CID;                                    /**< PDP Context ID. */
    bool Status;                                    /**< PDP Context enable status. */
} SIM7020_PDP_Status_t;

#endif /* SIM7020_PDP_DEFS_H_ */