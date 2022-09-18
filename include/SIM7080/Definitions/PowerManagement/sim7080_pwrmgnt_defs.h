 /*
 * sim7080_pwrmgnt_defs.h
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

#ifndef SIM7080_PWRMGNT_DEFS_H_
#define SIM7080_PWRMGNT_DEFS_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

#include "sim7080_defs.h"

/** @brief SIM7080 PSM Periodic-TAU definitions.
 */
typedef enum
{
    SIM7080_TAU_BASE_10_MIN = 0,                            /**< Base 10 minutes. */
    SIM7080_TAU_BASE_1_HOUR,                                /**< Base 1 hour. */
    SIM7080_TAU_BASE_10_HOUR,                               /**< Base 10 hours. */
    SIM7080_TAU_BASE_2_SEC,                                 /**< Base 2 seconds. */
    SIM7080_TAU_BASE_30_SECONDS,                            /**< Base 30 seconds. */
    SIM7080_TAU_BASE_1_MINUTE,                              /**< Base 1 minute. */
    SIM7080_TAU_BASE_320_H,                                 /**< Base 320 hours. */
} SIM7080_PSM_TAU_t;

/** @brief SIM7080 PSM Active-Time definitions.
 */
typedef enum
{
    SIM7080_TIME_BASE_2_SECONDS,                            /**< Base 2 seconds. */
    SIM7080_TIME_BASE_1_MINUTE,                             /**< Base 1 minute. */
    SIM7080_TIME_BASE_6_MINUTES,                            /**< Base 320 hours. */
} SIM7080_PSM_Time_t;

/** @brief SIM7080 PSM modem optimization object definition.
 */
typedef struct
{
    bool UsePSMEnter;                                       /**< Enable/disable PSM ENTER request without sending PSM_READY_REQ to NAS.
                                                                 This is a quick PSM operation. */
    bool UseOoS;                                            /**< Enable/disable Out of Service(OoS) status indication from Modem to AP. */
    bool UseLimitedStatus;                                  /**< Enable/disable limited service status indication from Modem to AP. */
    bool UseDeepSleep;                                      /**< Enable/disable deep-sleep mode.If PSM duration is less than the 
                                                                 threshold value.If enabled, it puts the device in deep-sleep mode,
                                                                 if PSM is not entered due to not meeting threshold value. */
    uint8_t MaxScans;                                       /**< Maximum number of full scans to wait before modem
                                                                 declares SYS_PSM_STATUS_OOS to clients.The range is from 1 to
                                                                 100.The default value is 2. */
    uint32_t PSMDuration;                                   /**< PSM duration used by PSM daemon upon OOS/Limited
                                                                 Service indication,due to service outage.The range is from 120 to
                                                                 4294967295. The default value is 120.The unit is second. */
    uint16_t Randomization;                                 /**< PSM wakeup randomization window to avoid network
                                                                 congestion due to all the PSM devices waking up at the same
                                                                 time. The Range is from 1 to 1000.The default value is 5. The unit is 5. */
    uint16_t MaxOOS;                                        /**< Maximum time in seconds to wait before declaring SYS_PSM_STATUS_OOS 
                                                                 to clients.The range is from 1 to 65535.The unit is second. */
    uint16_t EarlyWake;                                     /**< Device wakes up early to account for boot-up and acquisition delay.
                                                                 While programming PMIC,PSM daemon reduces PSM duration by this 
                                                                 duration.The range is from 1 to 1000. The default value is 3. The unit is second. */
} SIM7080_PSM_ModemOpts_t;

/** @brief SIM7080 PSM Network Registration status definition.
 */
typedef struct
{
    std::string tac;                                        /**< String type (string should be included in quotation marks); 
                                                                 two byte location area code in hexadecimal format 
                                                                 (e.g. "00C3" equals 195 in decimal). */
    std::string ci;                                         /**< String type (string should be included in quotation marks); 
                                                                 two bytes cell ID in hexadecimal format. */
    SIM7080_AcT_t Act;                                      /**< */
} SIM7080_PSM_NetReg_t;

#endif /* SIM7080_PWRMGNT_DEFS_H_ */