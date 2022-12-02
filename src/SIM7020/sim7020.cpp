 /*
 * sim7020.cpp
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

#if(CONFIG_SIMXX_DEV == 7020)

#include "sim7020.h"

#include "../Core/Events/sim70xx_evt.h"
#include "../Core/Queue/sim70xx_queue.h"
#include "../Core/Commands/sim70xx_commands.h"

#include "../Core/Arch/ESP32/GPIO/sim70xx_gpio.h"
#include "../Core/Arch/ESP32/UART/sim70xx_uart.h"
#include "../Core/Arch/ESP32/Timer/sim70xx_timer.h"
#include "../Core/Arch/ESP32/Logging/sim70xx_logging.h"
#include "../Core/Arch/ESP32/Watchdog/sim70xx_watchdog.h"

static const char* TAG = "SIM7020";

/** @brief          Perform the basic initialization of the device object.
 *  @param p_Device SIM7020 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
static SIM70XX_Error_t SIM7020_BasicInit(SIM7020_t& p_Device)
{
    p_Device.Internal.RxQueue = xQueueCreate(CONFIG_SIM70XX_QUEUE_LENGTH, sizeof(SIM70XX_CmdResp_t*));
    p_Device.Internal.TxQueue = xQueueCreate(CONFIG_SIM70XX_QUEUE_LENGTH, sizeof(SIM70XX_TxCmd_t*));
    p_Device.Internal.EventQueue = xQueueCreate(CONFIG_SIM70XX_QUEUE_LENGTH, sizeof(std::string*));
    if(p_Device.Internal.RxQueue == NULL)
    {
        goto SIM7020_BasicInit_NoRxQueue;
    }

    if(p_Device.Internal.TxQueue == NULL)
    {
        goto SIM7020_BasicInit_NoTxQueue;
    }

    if(p_Device.Internal.EventQueue == NULL)
    {
        goto SIM7020_BasicInit_NoEventQueue;
    }

    SIM70XX_GPIO_Init();

    if(SIM70XX_Evt_StartTask(p_Device.UART, &p_Device) == SIM70XX_ERR_OK)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_GPIO_Deinit();

SIM7020_BasicInit_NoEventQueue:
    vQueueDelete(p_Device.Internal.TxQueue);

SIM7020_BasicInit_NoTxQueue:
    vQueueDelete(p_Device.Internal.RxQueue);

SIM7020_BasicInit_NoRxQueue:
    return SIM70XX_ERR_NO_MEM;
}

SIM70XX_Error_t SIM7020_Init(SIM7020_t& p_Device, SIM7020_Config_t& p_Config, uint32_t Timeout)
{
    return SIM7020_Init(p_Device, p_Config, Timeout, SIM_BAUD_AUTO);
}

SIM70XX_Error_t SIM7020_Init(SIM7020_t& p_Device, SIM7020_Config_t& p_Config, SIM70XX_Baud_t Old)
{
    return SIM7020_Init(p_Device, p_Config, 10, Old);
}

SIM70XX_Error_t SIM7020_Init(SIM7020_t& p_Device, SIM7020_Config_t& p_Config, uint32_t Timeout, SIM70XX_Baud_t Old)
{
    if(p_Device.Internal.isInitialized)
    {
        SIM7020_Deinit(p_Device);
    }

    p_Device.UART.Interface = p_Config.UART.Interface;
    p_Device.UART.Rx = p_Config.UART.Rx;
    p_Device.UART.Tx = p_Config.UART.Tx;
    p_Device.UART.Baudrate = p_Config.UART.Baudrate;

    SIM70XX_ERROR_CHECK(SIM7020_BasicInit(p_Device));

    p_Device.Internal.isInitialized = true;

    if(Old != SIM_BAUD_AUTO)
    {
        SIM70XX_Error_t Error;

        SIM70XX_LOGI(TAG, "Changing baudrate...");

        p_Device.Internal.isInitialized = true;
        Error = SIM70XX_Tools_SetBaudrate(p_Device.UART, Old, p_Config.UART.Baudrate);
        p_Device.Internal.isInitialized = false;
        if(Error != SIM70XX_ERR_OK)
        {
            return Error;
        }
    }

	#ifdef CONFIG_SIM70XX_GPIO_USE_RESET
        SIM70XX_GPIO_HardReset();
    #else
        SIM70XX_ERROR_CHECK(SIM7020_SoftReset(p_Device, Timeout));
	#endif

    SIM70XX_ERROR_CHECK(SIM70XX_Tools_DisableEcho(p_Device.UART));

    SIM70XX_ERROR_CHECK(SIM7020_Ping(p_Device));
    SIM70XX_ERROR_CHECK(SIM7020_PSM_GetStatus(p_Device, &p_Device.PwrMgnt.PSM.isActive));

    if(p_Device.PwrMgnt.PSM.isActive)
    {
        SIM70XX_ERROR_CHECK(SIM7020_PSM_Disable(p_Device));
    }

    SIM70XX_ERROR_CHECK(SIM7020_GetFunctionality(p_Device))

    // TODO: Needs check with PDP context activation functions (SIM7020_Baerer_SetAPN)
    //SIM70XX_ERROR_CHECK(SIM7020_SetFunctionality(p_Device, SIM7020_FUNC_MIN));
   // SIM70XX_ERROR_CHECK(SIM7020_SetPSD(p_Device, SIM7020_PDP_IP, p_Config.APN));
    //SIM70XX_ERROR_CHECK(SIM7020_SetFunctionality(p_Device, SIM7020_FUNC_FULL));
    //SIM70XX_ERROR_CHECK(SIM7020_SetOperator(p_Device, SIM_MODE_MANUAL, p_Config.OperatorFormat, p_Config.Operator));
    SIM70XX_ERROR_CHECK(SIM7020_SetBand(p_Device, p_Config.Band));

    return SIM7020_Baerer_PDP_DisableAll(p_Device);
}

SIM70XX_Error_t SIM7020_Deinit(SIM7020_t& p_Device, bool Skip)
{
    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_OK;
    }

    // Shutdown the modem.
    if(Skip == false)
    {
        std::string Status;
        SIM70XX_TxCmd_t* Command;

        SIM70XX_CREATE_CMD(Command);
        *Command = SIM7020_AT_CPOWD(true);
        SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
        if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
        {
            return SIM70XX_ERR_FAIL;
        }

        SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, NULL, &Status);
        if(Status.find("NORMAL POWER DOWN") == std::string::npos)
        {
            return SIM70XX_ERR_FAIL;
        }
    }

    p_Device.Internal.isInitialized = false;

    SIM70XX_LOGI(TAG, "Modem inactive...");

    SIM70XX_Evt_StopTask(p_Device.UART);

    // Delete the message queues.
    vQueueDelete(p_Device.Internal.RxQueue);
    vQueueDelete(p_Device.Internal.TxQueue);
    vQueueDelete(p_Device.Internal.EventQueue);

    return SIM70XX_ERR_OK;
}

void SIM7020_PrepareSleep(SIM7020_t& p_Device)
{
    if(p_Device.Internal.isInitialized == false)
    {
        return;
    }

    ESP_LOGI(TAG, "Prepare driver for entering sleep mode...");

    SIM70XX_Evt_StopTask(p_Device.UART);

    p_Device.Internal.isInitialized = false;
}

SIM70XX_Error_t SIM7020_WakeUp(SIM7020_t& p_Device)
{
    std::string Response;

    if(p_Device.Internal.isInitialized == true)
    {
        return SIM70XX_ERR_OK;
    }

    ESP_LOGI(TAG, "Wake up driver from sleep mode...");

    SIM70XX_ERROR_CHECK(SIM7020_BasicInit(p_Device));

    p_Device.Internal.isInitialized = true;

    return SIM7020_Ping(p_Device);
}

SIM70XX_Error_t SIM7020_SoftReset(SIM7020_t& p_Device, uint32_t Timeout)
{
    uint32_t Now;
    std::string Response;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    if(p_Device.UART.TaskHandle != NULL)
    {
        SIM70XX_WDT_PAUSE_TASK(p_Device.UART.TaskHandle);
    }

    SIM70XX_LOGI(TAG, "Performing soft reset...");
    Now = SIM70XX_Timer_GetMilliseconds();
    do
    {
        // Reset the module.
        // NOTE: Echo mode is enabled after a reset!
        SIM70XX_UART_SendLine(p_Device.UART, "ATZ");
        Response = SIM70XX_UART_ReadStringUntil(p_Device.UART);
        Response = SIM70XX_UART_ReadStringUntil(p_Device.UART);
        SIM70XX_LOGD(TAG, "Response: %s", Response.c_str());

        // Check if the reset was successful.
        if(Response.find("OK") != std::string::npos)
        {
            SIM70XX_LOGI(TAG, "     Software reset successful!");

            if(p_Device.UART.TaskHandle != NULL)
            {
                SIM70XX_WDT_CONTINUE_TASK(p_Device.UART.TaskHandle);
            }

            return SIM70XX_ERR_OK;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    } while((SIM70XX_Timer_GetMilliseconds() - Now) < (Timeout * 1000UL));

    SIM70XX_LOGE(TAG, "     Software reset timeout!");

    if(p_Device.UART.TaskHandle != NULL)
    {
        SIM70XX_WDT_CONTINUE_TASK(p_Device.UART.TaskHandle);
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7020_SetPSD(SIM7020_t& p_Device, SIM7020_PDP_Type_t PDP, SIM70XX_APN_t APN)
{
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;
    SIM70XX_Error_t Error;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_ERROR_CHECK(SIM7020_SetFunctionality(p_Device, SIM7020_FUNC_MIN));

    if(PDP == SIM7020_PDP_IP)
    {
        CommandStr = "AT*MCGDEFCONT=\"IP\",\"" + APN.Name + "\"";
    }
    else if(PDP == SIM7020_PDP_IPV6)
    {
        CommandStr = "AT*MCGDEFCONT=\"IPV6\",\"" + APN.Name + "\"";
    }
    else if(PDP == SIM7020_PDP_IPV4V6)
    {
        CommandStr = "AT*MCGDEFCONT=\"IPV4V6\",\"" + APN.Name + "\"";
    }
    else if(PDP == SIM7020_PDP_NO_IP)
    {
        CommandStr = "AT*MCGDEFCONT=\"Non-IP\",\"" + APN.Name + "\"";
    }

    if((APN.Username.size() > 0) && (APN.Password.size() > 0))
    {
        CommandStr += ",\"" + APN.Username + "\",\"" + APN.Password + "\"";
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_MCGDEFCONT(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    // NOTE: We can not use the error macro here because Response and Status are swapped.
    Error = SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, NULL, &CommandStr);
    if(CommandStr.find("OK") == std::string::npos)
    {
        return Error;
    }

    return SIM7020_SetFunctionality(p_Device, SIM7020_FUNC_FULL);
}

SIM70XX_Error_t SIM7020_SetOperator(SIM7020_t& p_Device, SIM70XX_OpMode_t Mode, SIM70XX_OpForm_t Format, std::string Operator)
{
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    CommandStr = "AT+COPS=" + std::to_string(Mode);

    if((Mode == SIM_MODE_MANUAL) || (Mode == SIM_MODE_BOTH))
    {
        CommandStr += "," + std::to_string(Format) + ",\"" + Operator + "\"";
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_COPS_W(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

SIM70XX_Error_t SIM7020_GetOperator(SIM7020_t& p_Device, SIM70XX_Operator_t* const p_Operator, SIM70XX_OpMode_t* const p_Mode)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if((p_Operator == NULL) || (p_Mode == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device.Connection.Functionality != SIM7020_FUNC_FULL)
    {
        return SIM70XX_ERR_INVALID_STATE;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_COPS;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    SIM70XX_LOGD(TAG, "Response: %s", Response.c_str());

    if(Response.size())
    {
        *p_Mode = static_cast<SIM70XX_OpMode_t>(SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response)));
        p_Operator->Format = static_cast<SIM70XX_OpForm_t>(SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response)));

        if(p_Operator->Format == SIM_FORM_LONG)
        {
            p_Operator->Long = SIM70XX_Tools_SubstringSplitErase(&Response);
            p_Operator->Long.erase(std::remove(p_Operator->Long.begin(), p_Operator->Long.end(), '\"'), p_Operator->Long.end());
        }
        if(p_Operator->Format == SIM_FORM_SHORT)
        {
            p_Operator->Short = SIM70XX_Tools_SubstringSplitErase(&Response);
            p_Operator->Short.erase(std::remove(p_Operator->Short.begin(), p_Operator->Short.end(), '\"'), p_Operator->Short.end());
        }
        else
        {
            p_Operator->Numeric = SIM70XX_Tools_SubstringSplitErase(&Response);
            p_Operator->Numeric.erase(std::remove(p_Operator->Numeric.begin(), p_Operator->Numeric.end(), '\"'), p_Operator->Numeric.end());
        }

        p_Operator->Stat = SIM_OP_CUR;
        p_Operator->Act = static_cast<uint8_t>(SIM70XX_Tools_StringToUnsigned(Response));
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_GetAvailOperators(SIM7020_t& p_Device, std::vector<SIM70XX_Operator_t>* const p_Operators)
{
    size_t Start;
    std::string List;
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Operators == NULL) 
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device.Connection.Functionality != SIM7020_FUNC_FULL)
    {
        return SIM70XX_ERR_INVALID_STATE;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_COPS_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    SIM70XX_LOGD(TAG, "Response: %s", Response.c_str());

    // We start with the response
    //      "(2,\"D1\",\"TMO D\",\"26201\",9),(2,\"CHINA MOBILE\",\"CMCC\",\"46000\",0),(1,\"CHINA MOBILE\",\"CMCC\",\"46000\",9),,(0-4),(0-2)"
    // here.
    // Filter out the list of supported modes and formats.
    List = Response.substr(0, Response.find(",,"));

    // Process the remaining list.
    do
    {
        Start = List.find("(");
        if(Start != std::string::npos)
        {
            std::string Entry;
            SIM70XX_Operator_t Operator;

            // Get the next list entry.
            Entry = List.substr(Start + 1, List.find(")") - Start - 1);
            List.erase(Start, Entry.size() + 2);

            Operator.Stat = static_cast<SIM70XX_OpStat_t>(SIM70XX_Tools_StringToUnsigned((SIM70XX_Tools_SubstringSplitErase(&Entry))));
            Operator.Long = SIM70XX_Tools_SubstringSplitErase(&Entry);
            Operator.Long.erase(std::remove(Operator.Long.begin(), Operator.Long.end(), '\"'), Operator.Long.end());
            Operator.Short = SIM70XX_Tools_SubstringSplitErase(&Entry);
            Operator.Short.erase(std::remove(Operator.Short.begin(), Operator.Short.end(), '\"'), Operator.Short.end());
            Operator.Numeric = SIM70XX_Tools_SubstringSplitErase(&Entry);
            Operator.Numeric.erase(std::remove(Operator.Numeric.begin(), Operator.Numeric.end(), '\"'), Operator.Numeric.end());
            Operator.Act = static_cast<uint8_t>(SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Entry)));

            p_Operators->push_back(Operator);
        }
    } while(Start != std::string::npos);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_SetBand(SIM7020_t& p_Device, SIM7020_Band_t Band)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT_CBAND_W(Band);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

SIM70XX_Error_t SIM7020_GetBand(SIM7020_t& p_Device, SIM7020_Band_t* const p_Band)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Band == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device.Connection.Functionality == SIM7020_FUNC_MIN)
    {
        return SIM70XX_ERR_INVALID_STATE;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT_CBAND_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    *p_Band = static_cast<SIM7020_Band_t>(SIM70XX_Tools_StringToUnsigned(Response));

    SIM70XX_LOGD(TAG, "Band: %u", *p_Band);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_SetFunctionality(SIM7020_t& p_Device, SIM7020_Func_t Func, SIM7020_Reset_t Reset)
{
    uint32_t Now;
    std::string Line;
    std::string Response;
    SIM70XX_TxCmd_t Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device.Connection.Functionality == Func)
    {
        return SIM70XX_ERR_OK;
    }

    Command = SIM70XX_AT_CFUN_W(Func, Reset);
    SIM70XX_WDT_PAUSE_TASK(p_Device.UART.TaskHandle);
    SIM70XX_UART_SendLine(p_Device.UART, Command.Command);
    Now = SIM70XX_Timer_GetMilliseconds();
    do
    {
        Line = SIM70XX_UART_ReadStringUntil(p_Device.UART);
        Response += Line + "\n";

        if((SIM70XX_Timer_GetMilliseconds() - Now) > (Command.Timeout * 1000UL))
        {
            break;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    } while(((Response.find("CPIN") == std::string::npos) || (Response.find("OK") == std::string::npos)) &&(Response.find("ERROR") == std::string::npos));
    SIM70XX_WDT_CONTINUE_TASK(p_Device.UART.TaskHandle);

    if(Response.find("OK") == std::string::npos)
    {
        return SIM70XX_ERR_FAIL;
    }

    p_Device.Connection.Functionality = Func;

    SIM70XX_LOGI(TAG, "New functionality: %u", p_Device.Connection.Functionality);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_GetFunctionality(SIM7020_t& p_Device)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT_CFUN_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    p_Device.Connection.Functionality = static_cast<SIM7020_Func_t>(SIM70XX_Tools_StringToUnsigned(Response));

    SIM70XX_LOGI(TAG, "Functionality: %u", p_Device.Connection.Functionality);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_GetSIMStatus(SIM7020_t& p_Device, SIM7020_SIM_t* const p_Status)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Status == NULL)
    {
        return false;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return false;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT_CPIN_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return false;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    if(Response.find("READY") != std::string::npos)
    {
        *p_Status = SIM7020_SIM_READY;
    }
    else
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_ERR_OK;
}

bool SIM7020_isSIMReady(SIM7020_t& p_Device)
{
    SIM7020_SIM_t Status;

    if(SIM7020_GetSIMStatus(p_Device, &Status) != SIM70XX_ERR_OK)
    {
        return false;
    }

    if(Status == SIM7020_SIM_READY)
    {
        return true;
    }

    return false;      
}

SIM70XX_Error_t SIM7020_Ping(SIM7020_t& p_Device)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }

    if(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue) != SIM70XX_ERR_OK)
    {
        return SIM70XX_ERR_NOT_READY;
    }

    return SIM70XX_ERR_OK;
}

#endif