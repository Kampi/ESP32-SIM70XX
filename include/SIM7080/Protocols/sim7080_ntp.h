 /*
 * sim7080_ntp.h
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

#ifndef SIM7080_NTP_H_
#define SIM7080_NTP_H_

#include "sim7080_defs.h"
#include "sim70xx_errors.h"
#include "sim7080_ntp_defs.h"

/** @brief              Sync the local time with the time from an NTP server.
 *  @param p_Device     SIM7080 device object
 *  @param p_PDP        Pointer to PDP context
 *  @param Server       NTP server
 *  @param Timezone     Timezone
 *  @param p_Time       Pointer to local time
 *  @param p_Error      (Optional) NTP synchronization error code
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_NTP_Sync(SIM7080_t& p_Device, SIM7080_PDP_Context_t* p_PDP, std::string Server, int8_t TimeZone, struct tm* const p_Time, SIM7080_NTP_Error_t* const p_Error = NULL);

/** @brief              Get the current time from the module.
 *                      NOTE: Use \ref SIM7080_NTP_Sync to sync the time first!
 *  @param p_Device     SIM7080 device object
 *  @param p_Time       Pointer to local time
 *  @param p_Timezone   (Optional) Pointer to timezone
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_NTP_GetTime(SIM7080_t& p_Device, struct tm* const p_Time, int8_t* const p_Timezone = NULL);

#endif /* SIM7080_NTP_H_ */