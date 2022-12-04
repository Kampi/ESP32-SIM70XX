 /*
 * sim7080_fota.h
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

#ifndef SIM7080_FOTA_H_
#define SIM7080_FOTA_H_

#include "sim7080_defs.h"
#include "sim70xx_errors.h"
#include "sim7080_fota_defs.h"

/** @brief          Initialize the FOTA and download the firmware file to the AP by using HTTP.
 *  @param p_Device SIM7080 device object
 *  @param URL      Server URL
 *  @param Path     File path and name on AP side
 *  @param Timeout  (Optional) Request timeout in seconds
 *                  NOTE: Valid range is between 10 and 1000
 *  @param Retries  (Optional) Update retries
 *                  NOTE: Valid range is between 5 and 100
 *  @param p_Status (Optional) Pointer to HTTP status code
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_FOTA_HTTP_Init(SIM7080_t& p_Device, std::string URL, std::string Path, uint16_t Timeout = 50, uint8_t Retries = 5, SIM7080_FOTA_HTTP_Status_t* p_Status = NULL);

/** @brief          Execute the FOTA update.
 *  @param p_Device SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_FOTA_Execute(SIM7080_t& p_Device);

#endif /* SIM7080_FOTA_H_ */