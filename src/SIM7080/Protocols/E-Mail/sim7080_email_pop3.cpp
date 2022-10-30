 /*
 * sim7080_email_pop3.cpp
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
#include <esp_task_wdt.h>

#include "sim7080.h"
#include "sim7080_email.h"
#include "../../../Core/Arch/ESP32/UART/sim70xx_uart.h"
#include "../../../Core/Queue/sim70xx_queue.h"
#include "../../../Core/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7080_EMail";

/** @brief              Perform an error check.
 *  @param p_Response   Pointer to device response string
 *  @param p_Error      (Optional) Pointer to E-Mail transmission error code
 *  @return             SIM70XX_ERR_OK when successful
 *                      SIM70XX_ERR_MAIL_INVALID_RESPONSE when an e-mail error occurs. See \ref p_Error for more informations.
 */
static SIM70XX_Error_t SIM7080_EMail_POP3_ErrorCheck(std::string ErrorCode, SIM7080_EMail_Error_t* p_Error = NULL)
{
    SIM7080_EMail_Error_t ServerError;

    ServerError = (SIM7080_EMail_Error_t)SIM70XX_Tools_StringToUnsigned(ErrorCode);

    if(p_Error != NULL)
    {
        *p_Error = ServerError;
    }

    if(ServerError == 0)
    {
        return SIM70XX_ERR_MAIL_NOT_AVAILABLE;
    }
    else if(ServerError != SIM7080_EMAIL_OK)
    {
        return SIM70XX_ERR_MAIL_INVALID_RESPONSE;
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_EMail_POP3_Login(SIM7080_t& p_Device, SIM7080_EMail_Config_t* p_Config, SIM7080_EMail_Error_t* p_Error)
{
    uint8_t Timeout;
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if((p_Config == NULL) ||
       ((p_Config->Timeout > 0) && ((p_Config->Timeout < 10) || (p_Config->Timeout > 120))) ||
       (p_Config->SMTP_Address.size() == 0)
       )
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
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
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_POP3SRV(p_Config->SMTP_Address, p_Config->SMTP_Auth_User, p_Config->SMTP_Auth_Pass, p_Config->SMTP_Port);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_POP3IN;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+POP3IN", &Response) == false)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    Response.erase(Response.find("+POP3IN: "), std::string("+POP3IN: ").size());
    return SIM7080_EMail_POP3_ErrorCheck(Response, p_Error);
}

SIM70XX_Error_t SIM7080_EMail_POP3_ReadInbox(SIM7080_t& p_Device, uint32_t* p_Num, uint32_t* p_Size, SIM7080_EMail_Error_t* p_Error)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_POP3NUM
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    // Wait for the response.
    while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+POP3NUM:", &Response) == false)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    Response.erase(Response.find("+POP3NUM: "), std::string("+POP3NUM: ").size());
    SIM70XX_ERROR_CHECK(SIM7080_EMail_POP3_ErrorCheck(SIM70XX_Tools_SubstringSplitErase(&Response), p_Error));

    *p_Num = (uint32_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response));
    *p_Size = (uint32_t)SIM70XX_Tools_StringToUnsigned(Response);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_EMail_POP3_ReadEMailMeta(SIM7080_t& p_Device, uint32_t Mail, uint32_t* p_Size, std::string* p_ID, SIM7080_EMail_Error_t* p_Error)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_POP3LIST(Mail);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    // Wait for the response.
    while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+POP3LIST:", &Response) == false)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    Response.erase(Response.find("+POP3LIST: "), std::string("+POP3LIST: ").size());
    SIM70XX_ERROR_CHECK(SIM7080_EMail_POP3_ErrorCheck(SIM70XX_Tools_SubstringSplitErase(&Response), p_Error));

    // Remove the mail number.
    SIM70XX_Tools_SubstringSplitErase(&Response);

    *p_Size = (uint32_t)SIM70XX_Tools_StringToUnsigned(Response);

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_POP3UIDL(Mail);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+POP3UIDL:", &Response) == false)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    Response.erase(Response.find("+POP3UIDL: "), std::string("+POP3UIDL: ").size());
    SIM70XX_ERROR_CHECK(SIM7080_EMail_POP3_ErrorCheck(SIM70XX_Tools_SubstringSplitErase(&Response), p_Error));

    // Remove the E-Mail number.
    SIM70XX_Tools_SubstringSplitErase(&Response);

    *p_ID = Response;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_EMail_POP3_ReadEMail(SIM7080_t& p_Device, uint32_t ID, std::string* p_Mail, SIM7080_EMail_Error_t* p_Error, uint16_t PacketSize)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM70XX_Error_t Error = SIM70XX_ERR_OK;

    if((p_Mail == NULL) ||
       (PacketSize == 0) || (PacketSize > SIM7080_EMAIL_POP3_MAX_PAYLOAD_SIZE)
      )
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_POP3CMD(SIM7080_EMAIL_POP3_RETRIEVE, ID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    // Wait for the response.
    while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+POP3CMD:", &Response) == false)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    Response.erase(Response.find("+POP3CMD: "), std::string("+POP3CMD: ").size());
    SIM70XX_ERROR_CHECK(SIM7080_EMail_POP3_ErrorCheck(Response, p_Error));

    vTaskSuspend(p_Device.Internal.TaskHandle);
    do
    {
        uint8_t StatusCode;
        uint16_t BytesReceived;
        std::string Response;
        SIM70XX_TxCmd_t ReadCommand;

        esp_task_wdt_reset();

        ReadCommand = SIM7080_AT_POP3READ(PacketSize);
        SIM70XX_UART_SendLine(p_Device.UART, ReadCommand.Command);

        // Read the confirmation.
        //  <CR><LF>+POP3READ: ...<CR><LF>+
        do
        {
            Response = SIM70XX_UART_ReadStringUntil(p_Device.UART);
            vTaskDelay(10 / portTICK_PERIOD_MS);
        } while(Response.find('\r') == std::string::npos);
        Response = SIM70XX_UART_ReadStringUntil(p_Device.UART);

        // Some error occur and the connection is closed. Abort here.
        if(Response.find("POP3OUT") != std::string::npos)
        {
            Error = SIM70XX_ERR_FAIL;
            break;
        }

        ESP_LOGI(TAG, "Response: %s", Response.c_str());
        Response.erase(Response.find("+POP3READ: "), std::string("+POP3READ: ").size());
        StatusCode = (uint8_t)SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response));
        BytesReceived = (uint16_t)SIM70XX_Tools_StringToUnsigned(Response);

        ESP_LOGD(TAG, "Status code: %u", StatusCode);
        ESP_LOGD(TAG, "Bytes received: %u", BytesReceived);

        // An error has occured.
        if(BytesReceived == 0)
        {
            continue;
        }

        // Receive the data.
        do
        {
            if(SIM70XX_UART_Available(p_Device.UART))
            {
                char c;

                BytesReceived--;
                c = SIM70XX_UART_Read(p_Device.UART);
                p_Mail->push_back(c);
            }
        } while(BytesReceived > 0);

        // Read the trailing "OK" status code:
        //  <CR><LF><CR><LF>OK<CR><LF>
        do
        {
            Response = SIM70XX_UART_ReadStringUntil(p_Device.UART);
            vTaskDelay(10 / portTICK_PERIOD_MS);
        } while(Response.find('\r') == std::string::npos);
        SIM70XX_UART_ReadStringUntil(p_Device.UART);
        Response = SIM70XX_UART_ReadStringUntil(p_Device.UART);
        ESP_LOGD(TAG, "Response: %s", Response.c_str());

        // Addditional data are available.
        if(StatusCode == 1)
        {
        }
        // All data read.
        else if(StatusCode == 2)
        {
            break;
        }
        else if(StatusCode == 3)
        {
        }
    } while(true);
    vTaskResume(p_Device.Internal.TaskHandle);

    return Error;
}

SIM70XX_Error_t SIM7080_EMail_POP3_DeleteEMail(SIM7080_t& p_Device, uint32_t ID, SIM7080_EMail_Error_t* p_Error)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_POP3DEL(ID);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+POP3DEL", &Response) == false)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    Response.erase(Response.find("+POP3DEL: "), std::string("+POP3DEL: ").size());
    return SIM7080_EMail_POP3_ErrorCheck(Response, p_Error);
}

SIM70XX_Error_t SIM7080_EMail_POP3_Logout(SIM7080_t& p_Device, SIM7080_EMail_Error_t* p_Error)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_POP3OUT(Command->Timeout);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "+POP3OUT", &Response) == false)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    Response.erase(Response.find("+POP3OUT: "), std::string("+POP3OUT: ").size());
    return SIM7080_EMail_POP3_ErrorCheck(Response, p_Error);
}

#endif