 /*
 * sim7080_email.cpp
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

#include <sdkconfig.h>

#if((CONFIG_SIMXX_DEV == 7080) && (defined CONFIG_SIM70XX_DRIVER_WITH_EMAIL))

#include "sim7080.h"
#include "sim7080_email.h"
#include "../../../Core/Queue/sim70xx_queue.h"
#include "../../../Core/Commands/sim70xx_commands.h"

#include "../../../Core/Arch/ESP32/UART/sim70xx_uart.h"
#include "../../../Core/Arch/ESP32/Logging/sim70xx_logging.h"
#include "../../../Core/Arch/ESP32/Watchdog/sim70xx_watchdog.h"

static const char* TAG = "SIM7080_EMail";

/** @brief          Prepare an E-Mail and set the sender and the recipients.
 *  @param p_Device SIM7080 device object
 *  @param p_Config SIM7080 E-Mail configuration object
 *  @param p_Sender E-Mail sender
 *  @param p_To     To recipient
 *  @param Subject  E-Mail subject
 *  @param p_CC     CC recipent
 *  @param p_BCC    BCC recipient
 *  @return         SIM70XX_ERR_OK when successful
 */
static SIM70XX_Error_t SIM7080_EMail_PrepareEMail(SIM7080_t& p_Device, SIM7080_EMail_Config_t* const p_Config, SIM7080_EMail_User_t* const p_Sender, SIM7080_EMail_User_t* const p_To, std::string Subject, SIM7080_EMail_User_t* const p_CC, SIM7080_EMail_User_t* const p_BCC)
{
    uint8_t Timeout;
    SIM70XX_TxCmd_t* Command;

    if((p_Config == NULL) || (p_Sender == NULL) || (p_To == NULL) ||
       ((p_Config->Timeout != 0) && ((p_Config->Timeout < 10) || (p_Config->Timeout > 120))) ||
       (p_Config->SMTP_Address.size() == 0) ||
       (p_Sender->Address.size() > 48) || (p_Sender->Name.size() > 48) ||
       (p_To->Address.size() > 48) || (p_To->Name.size() > 48) ||
       ((p_CC != NULL) && ((p_CC->Address.size() > 48) || (p_CC->Name.size() > 48))) ||
       ((p_BCC != NULL) && ((p_BCC->Address.size() > 48) || (p_BCC->Name.size() > 48)))
       )
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    if(p_Config->Timeout == 0)
    {
        Timeout = 30;
    }
    else
    {
        Timeout = p_Config->Timeout;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_EMAILTO(Timeout);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMTPSRV(p_Config->SMTP_Address, p_Config->SMTP_Port);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMTPAUTH(p_Config->SMTP_Auth_User, p_Config->SMTP_Auth_Pass);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMTPFROM(p_Sender->Address, p_Sender->Name);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMTPRCPT(0, p_To->Address, p_To->Name);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMTPSUB(Subject);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    if(p_CC != NULL)
    {
        SIM70XX_CREATE_CMD(Command);
        *Command = SIM7080_AT_SMTPRCPT(1, p_CC->Address, p_CC->Name);
        SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
        if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
        {
            return SIM70XX_ERR_FAIL;
        }
        SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));
    }

    if(p_BCC != NULL)
    {
        SIM70XX_CREATE_CMD(Command);
        *Command = SIM7080_AT_SMTPRCPT(2, p_BCC->Address, p_BCC->Name);
        SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
        if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
        {
            return SIM70XX_ERR_FAIL;
        }
        SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_EMail_SendText(SIM7080_t& p_Device, SIM7080_PDP_Context_t* const p_PDP, SIM7080_EMail_Config_t* const p_Config, SIM7080_EMail_User_t* p_Sender, SIM7080_EMail_User_t* const p_To, std::string Subject, std::string Body, SIM7080_EMail_Error_t* const p_Error)
{
    return SIM7080_EMail_SendText(p_Device, p_PDP, p_Config, p_Sender, p_To, Subject, Body, NULL, NULL, p_Error);
}

SIM70XX_Error_t SIM7080_EMail_SendText(SIM7080_t& p_Device, SIM7080_PDP_Context_t* const p_PDP, SIM7080_EMail_Config_t* const p_Config, SIM7080_EMail_User_t* const p_Sender, SIM7080_EMail_User_t* const p_To, std::string Subject, std::string Body, SIM7080_EMail_User_t* const p_CC, SIM7080_EMail_User_t* p_BCC, SIM7080_EMail_Error_t* const p_Error)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM7080_EMail_Error_t MailError;

    if((p_PDP == NULL) || (p_Config == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_PDP->Internal.isActive == false)
    {
        return SIM70XX_ERR_PDP_NOT_ACTIVE;
    }

    // TODO: Add support for SNTP servers without authentication

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_EMAILCID(p_PDP->ID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    if(p_Config->isUTF8 == true)
    {
        Response = "UTF-8";
    }
    else
    {
        Response = "ASCII";
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMTPCS(Response);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_ERROR_CHECK(SIM7080_EMail_PrepareEMail(p_Device, p_Config, p_Sender, p_To, Subject, p_CC, p_BCC));
    
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMTPBODY(Body.size());
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, NULL, &Response);
    if(Response.find("DOWNLOAD") == std::string::npos)
    {
        return SIM70XX_ERR_FAIL;
    }

    SIM70XX_WDT_PAUSE_TASK(p_Device.UART.TaskHandle);
    SIM70XX_UART_Send(p_Device.UART, Body.c_str(), Body.size());
    SIM70XX_UART_ReadStringUntil(p_Device.UART);
    SIM70XX_UART_ReadStringUntil(p_Device.UART);
    SIM70XX_WDT_CONTINUE_TASK(p_Device.UART.TaskHandle);

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMTPSEND;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    // Wait for the transmission status.
    while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+SMTPSEND", &Response) == false)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    SIM70XX_LOGD(TAG, "Response: %s", Response.c_str());

    Response.erase(Response.find("+SMTPSEND: "), std::string("+SMTPSEND: ").size());

    MailError = static_cast<SIM7080_EMail_Error_t>(SIM70XX_Tools_StringToUnsigned(Response));
    if(p_Error != NULL)
    {
        *p_Error = MailError;
    }

    if(MailError != SIM7080_EMAIL_OK)
    {
        return SIM70XX_ERR_MAIL_INVALID_RESPONSE;
    }

    return SIM70XX_ERR_OK;
}

#ifdef CONFIG_SIM70XX_DRIVER_WITH_SSL
    SIM70XX_Error_t SIM7080_EMail_EnableSSL(SIM7080_t& p_Device, bool Enable, uint8_t SSL_Config, SIM7080_EMail_SSL_t SSL_Opts)
    {
        SIM70XX_TxCmd_t* Command;

        if(SSL_Config > 5)
        {
            return SIM70XX_ERR_INVALID_ARG;
        }
        else if(p_Device.Internal.isInitialized == false)
        {
            return SIM70XX_ERR_NOT_INITIALIZED;
        }

        SIM70XX_CREATE_CMD(Command);
        *Command = SIM7080_AT_EMAILSSL(SSL_Opts, SSL_Config);
        SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
        if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
        {
            return SIM70XX_ERR_FAIL;
        }
        SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

        return SIM70XX_ERR_OK;
    }
#endif

#endif