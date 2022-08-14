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
#include "sim7080_fs_defs.h"

/** @brief          Write a file into the file system.
 *  @param p_Device SIM7080 device object
 *  @param Path     Directory path
 *  @param Name     File name
 *  @param p_Size   Pointer to file size
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_FS_GetFileSize(SIM7080_t& p_Device, SIM7080_FS_Path_t Path, std::string Name, uint32_t* p_Size);

/** @brief          Write a file into the file system.
 *  @param p_Device SIM7080 device object
 *  @param Path     Directory path
 *  @param Name     File name
 *                  NOTE: Max. 230 characters are allowed!
 *  @param p_Buffer Pointer to data buffer
 *  @param Length   Buffer length
 *                  NOTE: 10240 bytes are allowed for a single file!
 *  @param Append   (Optional) If set to #true and the file already existed, add the data at the end of the file.
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_FS_Write(SIM7080_t& p_Device, SIM7080_FS_Path_t Path, std::string Name, const void* const p_Buffer, uint16_t Length, bool Append = false);

/** @brief              Read a from into the file system.
 *  @param p_Device     SIM7080 device object
 *  @param Path         Directory path
 *  @param Name         File name
 *                      NOTE: Max. 230 characters are allowed!
 *  @param p_Buffer     Pointer to data buffer
 *  @param Length       Number of bytes to read.
 *                      NOTE: 10240 bytes are allowed for a single file!
 *  @param UsePosition  (Optional) If set to #true the value from \ref Position is used as a read offset
 *  @param Position     (Optional) Read offset. Only used when \ref UsePosition is set to #true
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_FS_Read(SIM7080_t& p_Device, SIM7080_FS_Path_t Path, std::string Name, void* const p_Buffer, uint16_t Length, bool UsePosition = false, uint16_t Position = 0);

/** @brief          Delete a file.
 *  @param p_Device SIM7080 device object
 *  @param Path     Directory path
 *  @param Name     File name
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_FS_Delete(SIM7080_t& p_Device, SIM7080_FS_Path_t Path, std::string Name);

/** @brief          Rename a file.
 *  @param p_Device SIM7080 device object
 *  @param Path     Directory path
 *  @param Old      Old file name
 *  @param New      New file name
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_FS_Rename(SIM7080_t& p_Device, SIM7080_FS_Path_t Path, std::string Old, std::string New);

/** @brief          Get the free size of the file system.
 *  @param p_Device SIM7080 device object
 *  @param p_Free   Pointer to free file system size (bytes)
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_FS_GetFree(SIM7080_t& p_Device, uint32_t* const p_Free);

#endif /* SIM7080_FS_H_ */