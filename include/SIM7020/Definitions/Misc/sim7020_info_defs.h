 /*
 * sim7020_info_defs.h
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

#ifndef SIM7020_INFO_DEFS_H_
#define SIM7020_INFO_DEFS_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

/** @brief SIM7020 device information object definition.
 */
typedef struct
{
    std::string IMEI;                               /**< The International mobile equipment identifier (IMEI) of the attached SIM card.
                                                         NOTE: Managed by the device driver. */
    std::string ICCID;                              /**< The Integrated Circuit Card IDentifier (ICCID) of the attached SIM card.
                                                         NOTE: Managed by the device driver. */
    std::string Firmware;                           /**< Firmware version from the current device.
                                                         NOTE: Managed by the device driver. */
    std::string Manufacturer;                       /**< Manufacturer string.
                                                         NOTE: Managed by the device driver. */
    std::string Model;                              /**< Device model.
                                                         NOTE: Managed by the device driver. */
} SIM7020_Info_t;

#endif /* SIM7020_INFO_DEFS_H_ */