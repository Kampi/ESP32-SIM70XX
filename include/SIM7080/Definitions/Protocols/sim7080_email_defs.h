 /*
 * sim7080_email_defs.h
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

#ifndef SIM7080_EMAIL_DEFS_H_
#define SIM7080_EMAIL_DEFS_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

#include <sdkconfig.h>

#ifdef CONFIG_SIM70XX_DRIVER_WITH_SSL
    /** @brief SIM7080 E-Mail SSL enable options definitions.
     */
    typedef enum
    {
        SIM7080_EMAIL_SSL_NO    = 0,                        /**< Don´t use SSL. */
        SIM7080_EMAIL_SSL_ENABLE,                           /**< Enable SSL. */
        SIM7080_EMAIL_SSL_START,                            /**< Only for SMTP. */
    } SIM7080_EMail_SSL_t;
#endif

/** @brief SIM7080 E-Mail error codes.
 */
typedef enum
{
    SIM7080_EMAIL_OK            = 1,                        /**< The E-Mail has been sent successfully. */
    SIM7080_EMAIL_NET_ERR       = 61,                       /**< Network error. */
    SIM7080_EMAIL_DNS_ERR       = 62,                       /**< DNS resolve error. */
    SIM7080_EMAIL_SMTP_ERR      = 63,                       /**< SMTP TCP connection error. */
    SIM7080_EMAIL_TIMEOUT       = 64,                       /**< Timeout of SMTP server response. */
    SIM7080_EMAIL_RESP_ERR      = 65,                       /**< Server response error. */
    SIM7080_EMAIL_NO_AUTH       = 66,                       /**< Authentication failure. */
    SIM7080_EMAIL_AUTH_ERR      = 67,                       /**< Authentication failed. SMTP user name or password may be not right. */
    SIM7080_EMAIL_REC_ERR       = 68,                       /**< Bad recipient. */
    SIM7080_EMAIL_READ_TIMEOUT  = 69,                       /**< Timeout of read data. */
} SIM7080_EMail_Error_t;

/** @brief SIM7080 POP3 command codes.
 */
typedef enum
{
    SIM7080_EMAIL_POP3_LIST     = 1,                        /**< The "List" command returns a multi-line "scan listing". For each
                                                                 message on the maildrop list of the server the POP3 service returns a
                                                                 line containing the message number and its size in bytes. A final
                                                                 "dotline" will be printed at the end of the "scan listing". If there are no
                                                                 messages on the maildrop list of the server, the POP3 service returns
                                                                 a positive response, i.e. It does not issue an error response, but the
                                                                 "scan listing" will be empty. In either case, each scan listing will be
                                                                 finished by so-called "dotline", i.e. a new line with just a single dot.
                                                                 <msgNumber> and <lineNumber> must not be given. */
    SIM7080_EMAIL_POP3_UIDL,                                /**< The "Uidl" command returns a multi-line "unique-id Listing". For each
                                                                 message on the maildrop list of the Server the POP3 service returns a
                                                                 line containing the message number and its unique-id. A final "dotline"
                                                                 will be printed at the end of the "unique-id listing" If there are no
                                                                 messages on the maildrop list of the server. The POP3 service returns
                                                                 a positive response, i.e. It does not issue an error response, but the
                                                                 "unique-id listing" will be empty. In either case, each unique-id
                                                                 listing will be finished by so-called "dotline", i.e.a new line with just a
                                                                 singledot. <msgNumber> and <lineNumber> must not be given. */
    SIM7080_EMAIL_POP3_TOP,                                 /**< The command retrieves the number of lines of the message´s body
                                                                 from the POP3 server´s maildrop list. The POP3 server sends the
                                                                 headers of the message, the blank line separating the headers from
                                                                 the body, and then the number of lines of the message´s body. If the
                                                                 number of lines requested by The POP3 client is greater than the
                                                                 number of lines in the body, then the POP3 server sends the entire
                                                                 message. If no such message exists on the server the POP3 service
                                                                 issues an error response to the user. Each email will be finished by a
                                                                 so-called "dotline", i.e.a new line with just a single dot.
                                                                 <msgNumber> and <lineNumber> must be given. */
    SIM7080_EMAIL_POP3_RETRIEVE,                            /**< The command retrieves the related message from the POP3 server´s
                                                                 maildrop list. If no such message exists on the server the POP3
                                                                 service issues an error response to the user. Each email will be
                                                                 finished by a so-called "dotline", i.e. a new line with just a single dot. */
} SIM7080_EMail_POP3_Cmd_t;

/** @brief SIM7080 E-Mail user object.
 */
typedef struct
{
    std::string Address;                                    /**< The E-Mail address of the user. */
    std::string Name;                                       /**< The name of the user. */
} SIM7080_EMail_User_t;

/** @brief SIM7080 E-Mail configuration object.
 */
typedef struct
{
    uint8_t Timeout;                                        /**< SMTP / POP3 server response timeout in seconds.
                                                                 NOTE: Must be in the range between 10 and 120. Set to 0 to use the default value (30 s). */
    std::string SMTP_Address;                               /**< SMTP server address. */
    uint16_t SMTP_Port;                                     /**< SMTP server port. */
    std::string SMTP_Auth_User;                             /**< User name for the server authentication. */
    std::string SMTP_Auth_Pass;                             /**< Password for the server authentication. */
    bool isUTF8;                                            /**< Set to #true to use UTF-8 encoding. */
} SIM7080_EMail_Config_t;

#endif /* SIM7080_EMAIL_DEFS_H_ */