 /*
 * sim7020_pwrmgnt_defs.h
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

#ifndef SIM7020_PWRMGNT_DEFS_H_
#define SIM7020_PWRMGNT_DEFS_H_

#include <stdint.h>
#include <stdbool.h>

/** @brief SIM7020 PSM Periodic-TAU definitions.
 */
typedef enum
{
    SIM7020_TAU_BASE_10_M = 0,                              /**< Base 10 minutes. */
    SIM7020_TAU_BASE_1_H,                                   /**< Base 1 hour. */
    SIM7020_TAU_BASE_10_H,                                  /**< Base 10 hours. */
    SIM7020_TAU_BASE_2_S,                                   /**< Base 2 seconds. */
    SIM7020_TAU_BASE_30_S,                                  /**< Base 30 seconds. */
    SIM7020_TAU_BASE_1_M,                                   /**< Base 1 minute. */
    SIM7020_TAU_BASE_320_H,                                 /**< Base 320 hours. */
} SIM7020_PSM_TAU_t;

/** @brief SIM7020 PSM Active-Time definitions.
 */
typedef enum
{
    SIM7020_TIME_BASE_2_S,                                  /**< Base 2 seconds. */
    SIM7020_TIME_BASE_1_M,                                  /**< Base 1 minute. */
    SIM7020_TIME_BASE_6_M,                                  /**< Base 320 hours. */
} SIM7020_PSM_Time_t;

/** @brief PSM enable mode definitions.
 */
typedef enum
{
    SIM7020_PSM_DISABLE     = 0,                            /**< Disable the use of PSM. */
    SIM7020_PSM_ENABLE,                                     /**< Enable the use of PSM. */
    SIM7020_PSM_DISCARD,                                    /**< Disable the use of PSM and discard all parameters for PSM or, if available reset to the manufacturer specific default values. */
} SIM7020_PSM_Enable_t;

#endif /* SIM7020_PWRMGNT_DEFS_H_ */