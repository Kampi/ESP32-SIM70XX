 /*
 * sim7080_email.h
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

#ifndef SIM7080_EMAIL_H_
#define SIM7080_EMAIL_H_

#include "sim7080_defs.h"
#include "sim70xx_errors.h"
#include "sim7080_email_defs.h"

/** @brief          Send a text E-Mail.
 *  @param p_Device SIM7080 device object
 *  @param p_Config SIM7080 E-Mail configuration object
 *  @param p_Sender E-Mail sender
 *  @param p_To     To recipient
 *  @param Subject  E-Mail subject
 *  @param Body     E-Mail body
 *  @param p_Error  (Optional) Pointer to E-Mail transmission error code
 *  @param CID      (Optional) Context Identifier
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_EMail_SendText(SIM7080_t& p_Device, SIM7080_EMail_Config_t* p_Config, SIM7080_EMail_User_t* p_Sender, SIM7080_EMail_User_t* p_To, std::string Subject, std::string Body, SIM7080_EMail_Error_t* p_Error = NULL, uint8_t CID = 0);

/** @brief          Send a text E-Mail.
 *  @param p_Device SIM7080 device object
 *  @param p_Config SIM7080 E-Mail configuration object
 *  @param p_Sender E-Mail sender
 *  @param p_To     To recipient
 *  @param Subject  E-Mail subject
 *  @param Body     E-Mail body
 *  @param p_CC     CC recipent
 *  @param p_BCC    BCC recipient
 *  @param p_Error  (Optional) Pointer to E-Mail transmission error code
 *  @param CID      (Optional) Context Identifier
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_EMail_SendText(SIM7080_t& p_Device, SIM7080_EMail_Config_t* p_Config, SIM7080_EMail_User_t* p_Sender, SIM7080_EMail_User_t* p_To, std::string Subject, std::string Body, SIM7080_EMail_User_t* p_CC, SIM7080_EMail_User_t* p_BCC, SIM7080_EMail_Error_t* p_Error = NULL, uint8_t CID = 0);

#endif /* SIM7080_EMAIL_H_ */