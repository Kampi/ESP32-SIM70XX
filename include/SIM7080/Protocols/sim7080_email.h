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

/** @brief Maximum number of bytes for a single POP3 read transmission.
 */
#define SIM7080_EMAIL_POP3_MAX_PAYLOAD_SIZE                 1460

/** @brief          Send a text E-Mail.
 *  @param p_Device SIM7080 device object
 *  @param p_PDP    Pointer to PDP context
 *  @param p_Config SIM7080 E-Mail configuration object
 *  @param p_Sender E-Mail sender
 *  @param p_To     To recipient
 *  @param Subject  E-Mail subject
 *  @param Body     E-Mail body
 *  @param p_Error  (Optional) Pointer to E-Mail transmission error code
 *  @param CID      (Optional) Context Identifier
 *  @return         SIM70XX_ERR_OK when successful
 *                  SIM70XX_ERR_MAIL_INVALID_RESPONSE when an E-Mail error occurs. See \ref p_Error for more informations.
 */
SIM70XX_Error_t SIM7080_EMail_SendText(SIM7080_t& p_Device, SIM7080_PDP_Context_t* const p_PDP, SIM7080_EMail_Config_t* const p_Config, SIM7080_EMail_User_t* const p_Sender, SIM7080_EMail_User_t* const p_To, std::string Subject, std::string Body, SIM7080_EMail_Error_t* const p_Error = NULL);

/** @brief          Send a text E-Mail.
 *  @param p_Device SIM7080 device object
 *  @param p_PDP    Pointer to PDP context
 *  @param p_Config SIM7080 E-Mail configuration object
 *  @param p_Sender E-Mail sender
 *  @param p_To     To recipient
 *  @param Subject  E-Mail subject
 *  @param Body     E-Mail body
 *  @param p_CC     CC recipent
 *  @param p_BCC    BCC recipient
 *  @param p_Error  (Optional) Pointer to E-Mail transmission error code
 *  @return         SIM70XX_ERR_OK when successful
 *                  SIM70XX_ERR_MAIL_INVALID_RESPONSE when an E-Mail error occurs. See \ref p_Error for more informations.
 */
SIM70XX_Error_t SIM7080_EMail_SendText(SIM7080_t& p_Device, SIM7080_PDP_Context_t* const p_PDP, SIM7080_EMail_Config_t* const p_Config, SIM7080_EMail_User_t* const p_Sender, SIM7080_EMail_User_t* const p_To, std::string Subject, std::string Body, SIM7080_EMail_User_t* const p_CC, SIM7080_EMail_User_t* const p_BCC, SIM7080_EMail_Error_t* const p_Error = NULL);

/** @brief          Login to a POP3 server.
 *  @param p_Device SIM7080 device object
 *  @param p_Config SIM7080 E-Mail configuration object
 *  @param p_Error  (Optional) Pointer to E-Mail transmission error code
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_EMail_POP3_Login(SIM7080_t& p_Device, SIM7080_EMail_Config_t* const p_Config, SIM7080_EMail_Error_t* const p_Error = NULL);

/** @brief          Get the available E-Mails and the total size.
 *                  NOTE: You have to call \ref SIM7080_EMail_POP3_Login first
 *  @param p_Device SIM7080 device object
 *  @param p_Num    Pointer to number of E-Mails available
 *  @param p_Size   Pointer to total E-Mail size in bytes
 *  @param p_Error  (Optional) Pointer to E-Mail transmission error code
 *  @return         SIM70XX_ERR_OK when successful
 *                  SIM70XX_ERR_MAIL_INVALID_RESPONSE when an E-Mail error occurs. See \ref p_Error for more informations.
 */
SIM70XX_Error_t SIM7080_EMail_POP3_ReadInbox(SIM7080_t& p_Device, uint32_t* const p_Num, uint32_t* const p_Size, SIM7080_EMail_Error_t* const p_Error = NULL);

/** @brief          Get the unique ID and the size for a specific E-Mail.
 *                  NOTE: You have to call \ref SIM7080_EMail_POP3_Login first
 *  @param p_Device SIM7080 device object
 *  @param Mail     Target E-Mail index
 *  @param p_Size   Pointer to E-Mail size in bytes
 *  @param p_ID     Pointer to unique ID
 *  @param p_Error  (Optional) Pointer to E-Mail transmission error code
 *  @return         SIM70XX_ERR_OK when successful
 *                  SIM70XX_ERR_MAIL_INVALID_RESPONSE when an E-Mail error occurs. See \ref p_Error for more informations.
 */
SIM70XX_Error_t SIM7080_EMail_POP3_ReadEMailMeta(SIM7080_t& p_Device, uint32_t Mail, uint32_t* const p_Size, std::string* const p_ID, SIM7080_EMail_Error_t* const p_Error = NULL);

/** @brief              Get the unique ID and the size for a specific E-Mail.
 *                      NOTE: You have to call \ref SIM7080_EMail_POP3_Login first
 *  @param p_Device     SIM7080 device object
 *  @param ID           Target E-Mail index
 *  @param p_Mail       Pointer to E-Mail data
 *  @param p_Error      (Optional) Pointer to E-Mail transmission error code
 *  @param PacketSize   (Optional) Read packet size in bytes
 *                      NOTE: Only values between 1 and 1460 are allowed!
 *  @return             SIM70XX_ERR_OK when successful
 *                      SIM70XX_ERR_MAIL_INVALID_RESPONSE when an E-Mail error occurs. See \ref p_Error for more informations.
 */
SIM70XX_Error_t SIM7080_EMail_POP3_ReadEMail(SIM7080_t& p_Device, uint32_t ID, std::string* const p_Mail, SIM7080_EMail_Error_t* const p_Error = NULL, uint16_t PacketSize = SIM7080_EMAIL_POP3_MAX_PAYLOAD_SIZE);

/** @brief          Delete an E-Mail from the inbox.
 *                  NOTE: You have to call \ref SIM7080_EMail_POP3_Login first
 *  @param p_Device SIM7080 device object
 *  @param ID       Target E-Mail index
 *  @param p_Error  (Optional) Pointer to E-Mail transmission error code
 *  @return         SIM70XX_ERR_OK when successful
 *                  SIM70XX_ERR_MAIL_INVALID_RESPONSE when an E-Mail error occurs. See \ref p_Error for more informations.
 */
SIM70XX_Error_t SIM7080_EMail_POP3_DeleteEMail(SIM7080_t& p_Device, uint32_t ID, SIM7080_EMail_Error_t* const p_Error = NULL);

/** @brief          Logout from a POP3 server.
 *  @param p_Device SIM7080 device object
 *  @param p_Error  (Optional) Pointer to E-Mail transmission error code
 *  @return         SIM70XX_ERR_OK when successful
 *                  SIM70XX_ERR_MAIL_INVALID_RESPONSE when an E-Mail error occurs. See \ref p_Error for more informations.
 */
SIM70XX_Error_t SIM7080_EMail_POP3_Logout(SIM7080_t& p_Device, SIM7080_EMail_Error_t* const p_Error = NULL);

#ifdef CONFIG_SIM70XX_DRIVER_WITH_SSL
    /** @brief              Enable / Disable SSL support for the E-Mail client.
     *  @param p_Device     SIM7080 device object
     *  @param Enable       Enable / Disable SSL
     *  @param SSL_Config   (Optional) SSL configuration identifier
     *  @param SSL_Opts     (Optional) SSL options
     *  @return             SIM70XX_ERR_OK when successful
     */
    SIM70XX_Error_t SIM7080_EMail_EnableSSL(SIM7080_t& p_Device, bool Enable, uint8_t SSL_Config = 0, SIM7080_EMail_SSL_t SSL_Opts = SIM7080_EMAIL_SSL_ENABLE);
#endif

#endif /* SIM7080_EMAIL_H_ */