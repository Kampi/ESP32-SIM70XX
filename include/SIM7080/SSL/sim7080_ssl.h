 /*
 * sim7080_ssl.h
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

#ifndef SIM7080_SSL_H_
#define SIM7080_SSL_H_

#include "sim7080_defs.h"
#include "sim70xx_errors.h"
#include "sim7080_ssl_defs.h"

/** @brief          
 *  @param p_Device SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_SSL_Configure(SIM7080_t& p_Device);

/** @brief          Import the root certificate authority on the device.
 *  @param p_Device SIM7080 device object
 *  @param Path     Directory path
 *  @param RootCA   Client certificate file
 *  @param CID      (Optional) SSL CID
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_SSL_ImportRoot(SIM7080_t& p_Device, SIM7080_FS_Path_t Path, SIM7080_SSL_Type_t Type, SIM7080_SSL_File_t RootCA, uint8_t CID = 0);

/** @brief              Import the client key and the client certificate on the device.
 *  @param p_Device     SIM7080 device object
 *  @param Path         Directory path
 *  @param Client_Cer   Client certificate file
 *  @param Client_Key   Client key file
 *  @param CID          (Optional) SSL CID
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_SSL_ImportCert(SIM7080_t& p_Device, SIM7080_FS_Path_t Path, SIM7080_SSL_File_t Client_Cer, SIM7080_SSL_File_t Client_Key, uint8_t CID = 0);

#endif /* SIM7080_SSL_H_ */