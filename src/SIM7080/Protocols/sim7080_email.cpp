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

#include <esp_log.h>

#include "sim7080.h"
#include "sim7080_email.h"
#include "../../Private/UART/sim70xx_uart.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7080_EMail";

SIM70XX_Error_t SIM7080_EMail_SendText(SIM7080_t& p_Device, SIM7080_EMail_Config_t* p_Config, SIM7080_EMail_User_t* p_Sender, SIM7080_EMail_User_t* p_To, std::string Subject, std::string Body, SIM7080_EMail_Error_t* p_Error, uint8_t CID)
{
    return SIM7080_EMail_SendText(p_Device, p_Config, p_Sender, p_To, Subject, Body, NULL, NULL, p_Error, CID);
}

SIM70XX_Error_t SIM7080_EMail_SendText(SIM7080_t& p_Device, SIM7080_EMail_Config_t* p_Config, SIM7080_EMail_User_t* p_Sender, SIM7080_EMail_User_t* p_To, std::string Subject, std::string Body, SIM7080_EMail_User_t* p_CC, SIM7080_EMail_User_t* p_BCC, SIM7080_EMail_Error_t* p_Error, uint8_t CID)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM7080_EMail_Error_t Error;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if((p_Config == NULL) || (p_Sender == NULL) || (p_To == NULL) ||
            ((CID > 4) || (p_Sender->Address.size() > 48) || (p_Sender->Name.size() > 48) || (p_To->Address.size() > 48) || (p_To->Name.size() > 48)) ||
            ((p_CC != NULL) && ((p_CC->Address.size() > 48) || (p_CC->Name.size() > 48))) ||
            ((p_BCC != NULL) && ((p_BCC->Address.size() > 48) || (p_BCC->Name.size() > 48))))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    // TODO: Add support for SNTP servers without authentication

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_EMAILCID(CID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if((SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false) || (SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response) != SIM70XX_ERR_OK))
    {
        return SIM70XX_ERR_FAIL;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_EMAILTO(30);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if((SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false) || (SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response) != SIM70XX_ERR_OK))
    {
        return SIM70XX_ERR_FAIL;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMTPSRV(p_Config->SMTP_Address, p_Config->SMTP_Port);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if((SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false) || (SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response) != SIM70XX_ERR_OK))
    {
        return SIM70XX_ERR_FAIL;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMTPAUTH(p_Config->SMTP_Auth_User, p_Config->SMTP_Auth_Pass);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if((SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false) || (SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response) != SIM70XX_ERR_OK))
    {
        return SIM70XX_ERR_FAIL;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMTPFROM(p_Sender->Address, p_Sender->Name);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if((SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false) || (SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response) != SIM70XX_ERR_OK))
    {
        return SIM70XX_ERR_FAIL;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMTPRCPT(0, p_To->Address, p_To->Name);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if((SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false) || (SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response) != SIM70XX_ERR_OK))
    {
        return SIM70XX_ERR_FAIL;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMTPSUB(Subject);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if((SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false) || (SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response) != SIM70XX_ERR_OK))
    {
        return SIM70XX_ERR_FAIL;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMTPBODY(Body.size());
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, NULL, &Response);
    if(Response.find("DOWNLOAD") == std::string::npos)
    {
        return SIM70XX_ERR_FAIL;
    }

    vTaskSuspend(p_Device.Internal.TaskHandle);
    SIM70XX_UART_Send(p_Device.UART, Body.c_str(), Body.size());
    SIM70XX_UART_ReadStringUntil(p_Device.UART);
    SIM70XX_UART_ReadStringUntil(p_Device.UART);
    vTaskResume(p_Device.Internal.TaskHandle);

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SMTPSEND;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if((SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false) || (SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response) != SIM70XX_ERR_OK))
    {
        return SIM70XX_ERR_FAIL;
    }

    // Wait for the transmission status.
    while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+SMTPSEND:", &Response) == false)
    {
        vTaskDelay(100);
    }

    ESP_LOGI(TAG, "Response: %s", Response.c_str());

    Response.erase(Response.find("+SMTPSEND: "), std::string("+SMTPSEND: ").size());

    Error = (SIM7080_EMail_Error_t)std::stoi(Response);
    if(p_Error != NULL)
    {
        *p_Error = Error;
    }

    if(Error != SIM7080_EMAIL_ERR_OK)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_ERR_OK;
}

#endif