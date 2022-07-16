 /*
 * sim7020_info.h
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

#ifndef SIM7020_INFO_H_
#define SIM7020_INFO_H_

#include "sim7020_defs.h"
#include "sim70xx_errors.h"
#include "sim7020_info_defs.h"

/** @brief          Print the device informations.
 *  @param p_Info   Pointer to device information object
 */
void SIM7020_Info_Print(const SIM7020_Info_t* p_Info);

/** @brief          Read the device information.
 *  @param p_Device Pointer to SIM7020 device object
 *  @param p_Info   Pointer to device information object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Info_GetDeviceInformation(const SIM7020_t* const p_Device, SIM7020_Info_t* p_Info);

/** @brief                  Get the manufacturer of the device.
 *  @param p_Device         Pointer to SIM7020 device object
 *  @param p_Manufacturer   Pointer to manufacturer string
 *  @return                 SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Info_GetManufacturer(const SIM7020_t* const p_Device, std::string* p_Manufacturer);

/** @brief              Get the device model.
 *  @param p_Device     Pointer to SIM7020 device object
 *  @param p_Model      Pointer to model string
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Info_GetModel(const SIM7020_t* const p_Device, std::string* p_Model);

/** @brief              Get the version number of the SIM7020 device firmware.
 *  @param p_Device     Pointer to SIM7020 device object
 *  @param p_Firmware   Pointer to firmware string
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Info_GetFW(const SIM7020_t* const p_Device, std::string* p_Firmware);

/** @brief          Get the IMEI of the attached SIM card.
 *  @param p_Device Pointer to SIM7020 device object
 *  @param p_IMEI   Pointer to IMEI string
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Info_GetIMEI(const SIM7020_t* const p_Device, std::string* p_IMEI);

/** @brief          Get the ICCID of the attached SIM card.
 *  @param p_Device Pointer to SIM7020 device object
 *  @param p_ICCID  Pointer to ICCID string
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Info_GetICCID(const SIM7020_t* const p_Device, std::string* p_ICCID);

/** @brief          Get the network registration status.
 *  @param p_Device Pointer to SIM7020 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Info_GetNetworkRegistrationStatus(SIM7020_t* const p_Device );

/** @brief          Get a signal quality report.
 *  @param p_Device Pointer to SIM7020 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Info_GetQuality(SIM7020_t* const p_Device );

/** @brief          Get the network status.
 *  @param p_Device Pointer to SIM7020 device object
 *  @param p_Status Pointer to SIM7020 network status object
 *  @return         SIM7020_OK when successful
 */
SIM70XX_Error_t SIM7020_Info_GetNetworkStatus(const SIM7020_t* const p_Device, SIM7020_NetState_t* p_Status);

#endif /* SIM7020_INFO_H_ */