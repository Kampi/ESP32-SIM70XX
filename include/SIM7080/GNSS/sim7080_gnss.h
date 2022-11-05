 /*
 * sim7080_gnss.h
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

#ifndef SIM7080_GNSS_H_
#define SIM7080_GNSS_H_

#include "sim70xx_errors.h"
#include "sim7080_defs.h"
#include "sim7080_gnss_defs.h"

/** @brief          Configure the GPS module.
 *  @param p_Device SIM7080 device object
 *  @param p_Config Pointer to SIM7080 GPS configuration object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GNSS_Config(SIM7080_t& p_Device, const SIM7080_GNSS_Config_t* const p_Config);

/** @brief          Perform a cold start.
 *  @param p_Device SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GNSS_ColdStart(SIM7080_t& p_Device);

/** @brief          Perform a warm start.
 *  @param p_Device SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GNSS_WarmStart(SIM7080_t& p_Device);

/** @brief          Perform a hot start.
 *  @param p_Device SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GNSS_HotStart(SIM7080_t& p_Device);

/** @brief          Enable / Disable GNSS power.
 *  @param p_Device SIM7080 device object
 *  @param Enable   Enable / Disable
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GNSS_Enable(SIM7080_t& p_Device, bool Enable);

/** @brief          Check if the GPS module is enabled.
 *  @param p_Device SIM7080 device object
 *  @param p_Enable Pointer to power enable status
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GNSS_isEnabled(SIM7080_t& p_Device, bool* p_Enable);

/** @brief          Turn on GNSS and get location information once.
 *  @param p_Device SIM7080 device object
 *  @param p_Data   Pointer to GPS data
 *  @param Power    (Optional) Receiver power option
 *  @param p_Error  (Optional) Pointer to GPS error
 *  @return         SIM70XX_ERR_OK when successful
 *                  SIM70XX_ERR_INVALID_STATE when GNSS is already powered on. Call \ref SIM7080_GNSS_Enable to disable GNSS.
 */
SIM70XX_Error_t SIM7080_GNSS_GetSingleLocation(SIM7080_t& p_Device, SIM7080_GNSS_Data_t* p_Data, SIM7080_GNSS_PwrLevel_t Power = SIM7080_GNSS_PWR_FULL, SIM7080_GNSS_Error_t* p_Error = NULL);

/** @brief          Get the GNSS navigation information.
 *                  NOTE: You have to call \ref SIM7080_GNSS_Enable first to enable the GPS module power!
 *  @param p_Device SIM7080 device object
 *  @param p_Info   Pointer to GPS information
 *  @param Timeout  (Optional) Power on and lock timeout in seconds
 *  @return         SIM70XX_ERR_OK when successful
 *                  SIM70XX_ERR_INVALID_STATE when the GPS power is not enabled
 */
SIM70XX_Error_t SIM7080_GNSS_GetNavInfo(SIM7080_t& p_Device, SIM7080_GNSS_Info_t* p_Info, uint32_t Timeout = 120);

/** @brief              Turn on GNSS and start to get location informations.
 *  @param p_Device     SIM7080 device object
 *  @param Accuracy     (Optional) Accuracy settings
 *  @param minDistance  (Optional) Minimum distance in meters that must be traversed between position reports. 
 *                      Setting this interval to 0 will be a pure time-based tracking/batching.
 *  @param minInterval  (Optional) minInterval is the minimum time interval in milliseconds that must elapse between position reports
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GNSS_Start(SIM7080_t& p_Device, SIM7080_GNSS_Accuracy_t Accuracy = SIM7080_GNSS_ACCURACY_LOW, uint32_t minDistance = 0, uint32_t minInterval = 1000);

/** @brief          Turn off GNSS and stop listening for location informations.
 *  @param p_Device SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GNSS_Stop(SIM7080_t& p_Device);

#endif /* SIM7080_INFO_H_ */