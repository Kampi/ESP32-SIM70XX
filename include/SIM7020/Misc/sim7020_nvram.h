 /*
 * sim7020_nvram.h
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

#ifndef SIM7020_NVRAM_H_
#define SIM7020_NVRAM_H_

#include "sim7020_defs.h"
#include "sim70xx_errors.h"
#include "sim7020_nvram_defs.h"

/** @brief          Write data into the NVRAM.
 *  @param p_Device SIM7020 device object
 *  @param Key      Data key
 *  @param Data     Data string
 *  @param p_Error  (Optional) Pointer to NVRAM error code
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_NVRAM_Write(SIM7020_t& p_Device, std::string Key, const std::string Data, SIM7020_NVRAM_Error_t* p_Error = NULL);

/** @brief          Write data into the NVRAM.
 *  @param p_Device SIM7020 device object
 *  @param Key      Data key
 *  @param p_Buffer Pointer to data buffer
 *  @param Length   Data length
 *  @param p_Error  (Optional) Pointer to NVRAM error code
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_NVRAM_Write(SIM7020_t& p_Device, std::string Key, const uint8_t* p_Buffer, uint16_t Length, SIM7020_NVRAM_Error_t* p_Error = NULL);

/** @brief              Read data from the NVRAM.
 *  @param p_Device     SIM7020 device object
 *  @param Key          Data key
 *  @param p_Payload    Pointer to data
 *  @param p_Error      (Optional) Pointer to NVRAM error code
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_NVRAM_Read(SIM7020_t& p_Device, std::string Key, std::string* p_Payload, SIM7020_NVRAM_Error_t* p_Error = NULL);

/** @brief          Invalidate data from the NVRAM.
 *  @param p_Device SIM7020 device object
 *  @param Key      Data key
 *  @param p_Error  (Optional) Pointer to NVRAM error code
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_NVRAM_Erase(SIM7020_t& p_Device, std::string Key, SIM7020_NVRAM_Error_t* p_Error = NULL);

/** @brief          Get the data item IDs from the NVRAM.
 *  @param p_Device SIM7020 device object
 *  @param p_Keys   Pointer to data keys
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_NVRAM_GetKeys(SIM7020_t& p_Device, std::vector<std::string>* p_Keys);

#endif /* SIM7020_NVRAM_H_ */