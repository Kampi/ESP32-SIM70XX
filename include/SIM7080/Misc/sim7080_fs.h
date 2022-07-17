 /*
 * sim7080_fs.h
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

#ifndef SIM7080_FS_H_
#define SIM7080_FS_H_

#include "sim7020_defs.h"
#include "sim70xx_errors.h"
#include "Misc/sim7080_fs_defs.h"

/** @brief          Delete a file.
 *  @param p_Device SIM7080 device object
 *  @param Name     File name
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_FS_Delete(SIM7080_t& p_Device, std::string File);

/** @brief          Rename a file.
 *  @param p_Device SIM7080 device object
 *  @param Old      Old file name
 *  @param New      New file name
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_FS_Rename(SIM7080_t& p_Device, std::string Old, std::string New);

/** @brief          Get the free size of the file system.
 *  @param p_Device SIM7080 device object
 *  @param p_Free   Pointer to free file system size (bytes)
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_FS_GetFree(SIM7080_t& p_Device, uint32_t* const p_Free);

#endif /* SIM7080_FS_H_ */