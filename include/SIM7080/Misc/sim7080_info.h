 /*
 * sim7080_info.h
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

#ifndef SIM7080_INFO_H_
#define SIM7080_INFO_H_

#include "sim70xx_errors.h"
#include "Definitions/sim7080_defs.h"
#include "Definitions/Misc/sim7080_info_defs.h"

/** @brief          Print the device informations.
 *  @param p_Info   Pointer to device information object
 */
void SIM7080_Info_Print(SIM7080_Info_t* const p_Info);

/** @brief          Read the device information.
 *  @param p_Device SIM7080 device object
 *  @param p_Info   Pointer to device information object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Info_GetDeviceInformation(SIM7080_t& p_Device, SIM7080_Info_t* const p_Info);

/** @brief                  Get the manufacturer of the device.
 *  @param p_Device         SIM7080 device object
 *  @param p_Manufacturer   Pointer to manufacturer string
 *  @return                 SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Info_GetManufacturer(SIM7080_t& p_Device, std::string* const p_Manufacturer);

/** @brief              Get the device model.
 *  @param p_Device     SIM7080 device object
 *  @param p_Model      Pointer to model string
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Info_GetModel(SIM7080_t& p_Device, std::string* const p_Model);

/** @brief              Get the version number of the SIM7080 device firmware.
 *  @param p_Device     SIM7080 device object
 *  @param p_Firmware   Pointer to firmware string
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Info_GetFW(SIM7080_t& p_Device, std::string* const p_Firmware);

/** @brief          Get the IMEI of the attached SIM card.
 *  @param p_Device SIM7080 device object
 *  @param p_IMEI   Pointer to IMEI string
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Info_GetIMEI(SIM7080_t& p_Device, std::string* const p_IMEI);

/** @brief          Get the ICCID of the attached SIM card.
 *  @param p_Device SIM7080 device object
 *  @param p_ICCID  Pointer to ICCID string
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Info_GetICCID(SIM7080_t& p_Device, std::string* const p_ICCID);

/** @brief          Get the network registration status.
 *  @param p_Device SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Info_GetNetworkRegistrationStatus(SIM7080_t& p_Device);

/** @brief          Get a signal quality report.
 *  @param p_Device SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Info_GetQuality(SIM7080_t& p_Device);

/** @brief          Get the User Equipment informations.
 *  @param p_Device SIM7080 device object
 *  @param p_Info   Pointer to UE system information object
 *  @return         SIM70XX_ERR_OK when successful
 *                  SIM70XX_ERR_NOT_READY when no service is available
 */
SIM70XX_Error_t SIM7080_Info_GetEquipmentInfo(SIM7080_t& p_Device, SIM7080_UEInfo_t* p_Info);

#endif /* SIM7080_INFO_H_ */