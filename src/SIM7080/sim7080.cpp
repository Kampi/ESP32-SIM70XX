 /*
 * sim7080.cpp
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

#if(CONFIG_SIMXX_DEV == 7080)

#include "sim7080.h"

#include "../Core/Events/sim70xx_evt.h"
#include "../Core/Queue/sim70xx_queue.h"
#include "../Core/Commands/sim7080_commands.h"

#include "../Core/Arch/ESP32/GPIO/sim70xx_gpio.h"
#include "../Core/Arch/ESP32/UART/sim70xx_uart.h"
#include "../Core/Arch/ESP32/Timer/sim70xx_timer.h"
#include "../Core/Arch/ESP32/Logging/sim70xx_logging.h"
#include "../Core/Arch/ESP32/Watchdog/sim70xx_watchdog.h"

static const char* TAG = "SIM7080";

SIM70XX_Error_t SIM7080_Init(SIM7080_t& p_Device, const SIM7080_Config_t& p_Config, uint32_t Timeout)
{
    return SIM7080_Init(p_Device, p_Config, Timeout, SIM_BAUD_AUTO);
}

SIM70XX_Error_t SIM7080_Init(SIM7080_t& p_Device, const SIM7080_Config_t& p_Config, SIM70XX_Baud_t Old)
{
    return SIM7080_Init(p_Device, p_Config, 10, Old);
}

SIM70XX_Error_t SIM7080_Init(SIM7080_t& p_Device, const SIM7080_Config_t& p_Config, uint32_t Timeout, SIM70XX_Baud_t Old)
{
    if(p_Device.Internal.isInitialized)
    {
        SIM7080_Deinit(p_Device);
    }

    p_Device.Internal.RxQueue = xQueueCreate(CONFIG_SIM70XX_QUEUE_LENGTH, sizeof(SIM70XX_CmdResp_t*));
    p_Device.Internal.TxQueue = xQueueCreate(CONFIG_SIM70XX_QUEUE_LENGTH, sizeof(SIM70XX_TxCmd_t*));
    p_Device.Internal.EventQueue = xQueueCreate(CONFIG_SIM70XX_QUEUE_LENGTH, sizeof(std::string*));
    if((p_Device.Internal.RxQueue == NULL) || (p_Device.Internal.TxQueue == NULL) || (p_Device.Internal.EventQueue == NULL))
    {
        return SIM70XX_ERR_NO_MEM;
    }

    #ifdef CONFIG_SIM70XX_DRIVER_WITH_GNSS
        p_Device.GNSS.EventQueue = xQueueCreate(CONFIG_SIM70XX_QUEUE_LENGTH, sizeof(SIM7080_GNSS_Data_t*));
        if(p_Device.GNSS.EventQueue == NULL)
        {
            return SIM70XX_ERR_NO_MEM;
        }
        p_Device.GNSS.isListening = false;
    #endif

    p_Device.UART.Interface = p_Config.UART.Interface;
    p_Device.UART.Rx = p_Config.UART.Rx;
    p_Device.UART.Tx = p_Config.UART.Tx;
    p_Device.UART.Baudrate = p_Config.UART.Baudrate;

    SIM70XX_GPIO_Init();

    SIM70XX_ERROR_CHECK(SIM70XX_Evt_StartTask(p_Device.UART, &p_Device));

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

	SIM70XX_ERROR_CHECK(SIM7080_SoftReset(p_Device, Timeout));

    SIM70XX_ERROR_CHECK(SIM70XX_Tools_DisableEcho(p_Device.UART));

    SIM70XX_ERROR_CHECK(SIM7080_Ping(p_Device));
    SIM70XX_ERROR_CHECK(SIM7080_GetFunctionality(p_Device));
    SIM70XX_ERROR_CHECK(SIM7080_SetFunctionality(p_Device, SIM7080_FUNC_FULL));

    #ifdef CONFIG_SIM70XX_DRIVER_WITH_FS
        SIM7080_FS_Deinit(p_Device);
        SIM70XX_ERROR_CHECK(SIM7080_FS_GetFree(p_Device, &p_Device.FS.Free));
    #endif

    SIM70XX_ERROR_CHECK(SIM7080_SetNetMode(p_Device, p_Config.NetMode));
    SIM70XX_ERROR_CHECK(SIM7080_SetSelectedMode(p_Device, p_Config.Mode));

    if(p_Config.Mode == SIM7080_MODE_BOTH)
    {
        SIM70XX_ERROR_CHECK(SIM7080_SetBandConfig(p_Device, SIM7080_MODE_CAT, p_Config.Bandlist));
        SIM70XX_ERROR_CHECK(SIM7080_SetBandConfig(p_Device, SIM7080_MODE_NB, p_Config.Bandlist));
    }
    else
    {
        SIM70XX_ERROR_CHECK(SIM7080_SetBandConfig(p_Device, p_Config.Mode, p_Config.Bandlist));
    }

    // Disable all PDP contextes.
    for(uint8_t i = 0; i < 4; i++)
    {
        SIM7080_PDP_IP_Action(p_Device, i, SIM7080_PDP_DISABLE);
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_Deinit(SIM7080_t& p_Device, bool Skip)
{
    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_OK;
    }

    // Shutdown the modem.
    if(Skip == false)
    {
        std::string Response;
        SIM70XX_TxCmd_t* Command;

        SIM70XX_CREATE_CMD(Command);
        *Command = SIM7080_AT_CPOWD(true);
        SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
        if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
        {
            return SIM70XX_ERR_FAIL;
        }

        SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response);
        if(Response.find("NORMAL POWER DOWN") == std::string::npos)
        {
            return SIM70XX_ERR_FAIL;
        }
    }

    p_Device.Internal.isInitialized = false;

    SIM70XX_LOGI(TAG, "Modem inactive...");

    SIM70XX_Evt_StopTask(NULL);
    SIM70XX_Evt_StopTask(p_Device.UART.TaskHandle);
    p_Device.UART.TaskHandle = NULL;

    // Delete the message queues.
    #ifdef CONFIG_SIM70XX_DRIVER_WITH_GNSS
        vQueueDelete(p_Device.GNSS.EventQueue);
    #endif

    vQueueDelete(p_Device.Internal.RxQueue);
    vQueueDelete(p_Device.Internal.TxQueue);
    vQueueDelete(p_Device.Internal.EventQueue);

    // Deinitialize the UART interface.
    return SIM70XX_UART_Deinit(p_Device.UART);
}

SIM70XX_Error_t SIM7080_SoftReset(SIM7080_t& p_Device, uint32_t Timeout)
{
    uint32_t Now;
    std::string Response;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device.UART.TaskHandle != NULL)
    {
        vTaskSuspend(p_Device.UART.TaskHandle);
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
        SIM70XX_LOGI(TAG, "Response: %s", Response.c_str());

        // Check if the reset was successful.
        if(Response.find("OK") != std::string::npos)
        {
            SIM70XX_LOGI(TAG, "     Software reset successful!");

            if(p_Device.UART.TaskHandle != NULL)
            {
                vTaskResume(p_Device.UART.TaskHandle);
            }

            return SIM70XX_ERR_OK;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    } while((SIM70XX_Timer_GetMilliseconds() - Now) < (Timeout * 1000UL));

    SIM70XX_LOGE(TAG, "     Software reset timeout!");

    if(p_Device.UART.TaskHandle != NULL)
    {
        vTaskResume(p_Device.UART.TaskHandle);
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7080_IP_AutoAPN(SIM7080_t& p_Device, SIM70XX_APN_t APN, uint8_t PDP, uint32_t Timeout)
{
    unsigned long Now;
    bool isAttached;

    if(PDP > 3)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    if(p_Device.Connection.Functionality != SIM7080_FUNC_MIN)
    {
        SIM70XX_ERROR_CHECK(SIM7080_SetFunctionality(p_Device, SIM7080_FUNC_MIN));
    }
    SIM70XX_ERROR_CHECK(SIM7080_SetFunctionality(p_Device, SIM7080_FUNC_FULL));

    isAttached = false;
    Now = SIM70XX_Timer_GetMilliseconds();
    do
    {
        SIM70XX_Qual_t Report;

        SIM70XX_ERROR_CHECK(SIM7080_Info_GetQuality(p_Device, &Report));
        if(Report.RSSI != 99)
        {
            isAttached = SIM7080_PGP_GRPS_isAttached(p_Device);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } while((isAttached == false) && ((SIM70XX_Timer_GetMilliseconds() - Now) < (Timeout * 1000)));

    if(isAttached == false)
    {
        return SIM70XX_ERR_TIMEOUT;
    }

    //SIM70XX_ERROR_CHECK(SIM7080_GetOperator(p_Device));
    SIM70XX_ERROR_CHECK(SIM7080_PDP_IP_Configure(p_Device, SIM7080_PDP_IP_IP, APN, PDP));

    return SIM7080_PDP_IP_Action(p_Device, PDP, SIM7080_PDP_ENABLE);
}

SIM70XX_Error_t SIM7080_IP_ManualAPN(SIM7080_t& p_Device, SIM70XX_APN_t APN, uint8_t PDP, uint32_t Timeout)
{
    if(PDP > 3)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    if(p_Device.Connection.Functionality != SIM7080_FUNC_MIN)
    {
        SIM70XX_ERROR_CHECK(SIM7080_SetFunctionality(p_Device, SIM7080_FUNC_MIN));
    }

    // TODO: Needs check. It doesn´t work

    SIM70XX_ERROR_CHECK(SIM7080_PDP_GPRS_Define(p_Device, SIM7080_PDP_GPRS_IP, APN, PDP));
    SIM70XX_ERROR_CHECK(SIM7080_SetFunctionality(p_Device, SIM7080_FUNC_FULL));

    return SIM7080_IP_AutoAPN(p_Device, APN, PDP, Timeout);
}

SIM70XX_Error_t SIM7080_SetOperator(SIM7080_t& p_Device, SIM70XX_OpMode_t Mode, SIM70XX_OpForm_t Format, std::string Operator, SIM7080_AcT_t AcT)
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
        CommandStr += "," + std::to_string(Format) + ",\"" + Operator + "\"" + "," + std::to_string(AcT);
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_COPS_W(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

SIM70XX_Error_t SIM7080_GetOperator(SIM7080_t& p_Device, SIM70XX_Operator_t* p_Operator, SIM70XX_OpMode_t* p_Mode, SIM70XX_OpForm_t* p_Format)
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

    // The device must be active to check the operators.
    SIM70XX_ERROR_CHECK(SIM7080_SetFunctionality(p_Device, SIM7080_FUNC_FULL));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_COPS;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    SIM70XX_LOGI(TAG, "Response: %s", Response.c_str());

    if(Response.size())
    {
        *p_Mode = (SIM70XX_OpMode_t)stoi(SIM70XX_Tools_SubstringSplitErase(&Response));
        p_Operator->Format = (SIM70XX_OpForm_t)stoi(SIM70XX_Tools_SubstringSplitErase(&Response));

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
        p_Operator->Act = stoi(SIM70XX_Tools_SubstringSplitErase(&Response));
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_GetAvailOperators(SIM7080_t& p_Device, std::vector<SIM70XX_Operator_t>* p_Operators)
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

    // The device must be active to check the operators.
    SIM70XX_ERROR_CHECK(SIM7080_SetFunctionality(p_Device, SIM7080_FUNC_FULL));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_COPS_R;
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

    // Process the remaining list
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

            Operator.Stat = (SIM70XX_OpStat_t)stoi(SIM70XX_Tools_SubstringSplitErase(&Entry));
            Operator.Long = SIM70XX_Tools_SubstringSplitErase(&Entry);
            Operator.Long.erase(std::remove(Operator.Long.begin(), Operator.Long.end(), '\"'), Operator.Long.end());
            Operator.Short = SIM70XX_Tools_SubstringSplitErase(&Entry);
            Operator.Short.erase(std::remove(Operator.Short.begin(), Operator.Short.end(), '\"'), Operator.Short.end());
            Operator.Numeric = SIM70XX_Tools_SubstringSplitErase(&Entry);
            Operator.Numeric.erase(std::remove(Operator.Numeric.begin(), Operator.Numeric.end(), '\"'), Operator.Numeric.end());
            Operator.Act = stoi(SIM70XX_Tools_SubstringSplitErase(&Entry));

            p_Operators->push_back(Operator);
        }
    } while(Start != std::string::npos);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_SetBandConfig(SIM7080_t& p_Device, SIM7080_Mode_t Mode, std::vector<uint8_t> Bands)
{
    std::string ModeStr;
    std::string Bandlist;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    for(uint8_t i = 0; i < (Bands.size() - 1); i++)
    {
        Bandlist += std::to_string(Bands.at(i)) + ",";
    }
    Bandlist += std::to_string(Bands.at((Bands.size() - 1)));

    if(Mode == SIM7080_MODE_NB)
    {
        ModeStr = "NB-IOT";
    }
    else
    {
        ModeStr = "CAT-M";
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT_CBANDCFG_W(ModeStr, Bandlist);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_GetBandConfig(SIM7080_t& p_Device, SIM7080_Mode_t Mode, std::vector<uint8_t>* p_Bands)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Bands == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    // TODO: Not finished

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT_CBANDCFG_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_SetBand(SIM7080_t& p_Device, SIM7080_Band_t Band)
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
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_GetBand(SIM7080_t& p_Device, SIM7080_Band_t* p_Band)
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
    else if(p_Device.Connection.Functionality == SIM7080_FUNC_MIN)
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

    if(Response.find("EGSM_MODE") != std::string::npos)
    {
        *p_Band = SIM7080_BAND_EGSM;
    }
    else if(Response.find("DCS_MODE") != std::string::npos)
    {
        *p_Band = SIM7080_BAND_DCS;
    }
    else if(Response.find("ALL_MODE") != std::string::npos)
    {
        *p_Band = SIM7080_BAND_ALL;
    }
    else
    {
        *p_Band = (SIM7080_Band_t)SIM70XX_Tools_StringToUnsigned(Response);
    }

    SIM70XX_LOGD(TAG, "Band: %u", *p_Band);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_SetNetMode(SIM7080_t& p_Device, SIM7080_NetMode_t Mode)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CNMP_W(Mode);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_GetNetMode(SIM7080_t& p_Device, SIM7080_NetMode_t* p_Mode)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Mode == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device.Connection.Functionality == SIM7080_FUNC_MIN)
    {
        return SIM70XX_ERR_INVALID_STATE;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CNMP_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    *p_Mode = (SIM7080_NetMode_t)SIM70XX_Tools_StringToUnsigned(Response);

    SIM70XX_LOGD(TAG, "Mode: %u", *p_Mode);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_SetSelectedMode(SIM7080_t& p_Device, SIM7080_Mode_t Mode)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CMNB_W(Mode);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_GetSelectedMode(SIM7080_t& p_Device, SIM7080_Mode_t* p_Mode)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Mode == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device.Connection.Functionality == SIM7080_FUNC_MIN)
    {
        return SIM70XX_ERR_INVALID_STATE;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CMNB_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    *p_Mode = (SIM7080_Mode_t)SIM70XX_Tools_StringToUnsigned(Response);

    SIM70XX_LOGD(TAG, "Selected mode: %u", *p_Mode);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_SetFunctionality(SIM7080_t& p_Device, SIM7080_Func_t Func, SIM7080_Reset_t Reset)
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

    // TODO: Copy from SIM7020

    Command = SIM70XX_AT_CFUN_W(Func, Reset);
    vTaskSuspend(p_Device.UART.TaskHandle);
    SIM70XX_LOGI(TAG, "Tranmit command: %s", Command.Command.c_str());
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
    } while(Line.size() > 0);
    vTaskResume(p_Device.UART.TaskHandle);

    SIM70XX_LOGI(TAG, "Response: %s", Response.c_str());

    if(Response.find("OK") == std::string::npos)
    {
        return SIM70XX_ERR_FAIL;
    }

    p_Device.Connection.Functionality = Func;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_GetFunctionality(SIM7080_t& p_Device)
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

    p_Device.Connection.Functionality = (SIM7080_Func_t)SIM70XX_Tools_StringToUnsigned(Response);

    SIM70XX_LOGI(TAG, "Functionality: %u", p_Device.Connection.Functionality);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_GetSIMStatus(SIM7080_t& p_Device, SIM7080_SIM_t* const p_Status)
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
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    if(Response.find("READY") != std::string::npos)
    {
        *p_Status = SIM7080_SIM_READY;
    }
    else if(Response.find("SIM PIN") != std::string::npos)
    {
        *p_Status = SIM7080_SIM_WAIT_PIN;
    }
    else if(Response.find("SIM PUK") != std::string::npos)
    {
        *p_Status = SIM7080_SIM_WAIT_PUK;
    }
    else if(Response.find("PH_SIM PIN") != std::string::npos)
    {
        *p_Status = SIM7080_SIM_WAIT_PH_PIN;
    }
    else if(Response.find("PH_SIM PUK") != std::string::npos)
    {
        *p_Status = SIM7080_SIM_WAIT_PH_PUK;
    }
    else if(Response.find("PH_SIM NET") != std::string::npos)
    {
        *p_Status = SIM7080_SIM_WAIT_NET_PIN;
    }
    else if(Response.find("SIM PIN2") != std::string::npos)
    {
        *p_Status = SIM7080_SIM_WAIT_PIN2;
    }
    else if(Response.find("SIM PUK2") != std::string::npos)
    {
        *p_Status = SIM7080_SIM_WAIT_PUK2;
    }
    else
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_ERR_OK;
}

bool SIM7080_isSIMReady(SIM7080_t& p_Device)
{
    SIM7080_SIM_t Status;

    if(SIM7080_GetSIMStatus(p_Device, &Status) != SIM70XX_ERR_OK)
    {
        return false;
    }

    if(Status == SIM7080_SIM_READY)
    {
        return true;
    }

    return false;   
}

SIM70XX_Error_t SIM7080_Ping(SIM7080_t& p_Device)
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