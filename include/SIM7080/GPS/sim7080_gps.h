 /*
 * sim7080_gps.h
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

#ifndef SIM7080_GPS_H_
#define SIM7080_GPS_H_

#include "sim70xx_errors.h"
#include "sim7080_defs.h"
#include "sim7080_gps_defs.h"

/** @brief          Configure the GPS module.
 *  @param p_Device SIM7080 device object
 *  @param p_Config Pointer to SIM7080 GPS configuration object
 *  @return         #true when the module is initialized
 */
SIM70XX_Error_t SIM7080_GPS_Config(SIM7080_t& p_Device, const SIM7080_GPS_Config_t const* p_Config);

/** @brief          
 *  @param p_Device SIM7080 device object
 *  @param Power    
 *  @param p_Error  (Optional) 
 *  @return         #true when the module is initialized
 */
SIM70XX_Error_t SIM7080_GPS_GetSingleLocation(SIM7080_t& p_Device, SIM7080_GPS_PwrLevel_t Power, SIM7080_GPS_Error_t* p_Error = NULL);

#endif /* SIM7080_INFO_H_ */