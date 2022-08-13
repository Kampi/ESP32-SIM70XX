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

#include <esp_log.h>

#include "sim7080.h"
#include "../Private/UART/sim70xx_uart.h"
#include "../Private/Events/sim70xx_evt.h"
#include "../Private/Queue/sim70xx_queue.h"
#include "../Private/Commands/sim7080_commands.h"

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
    std::string Response;

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

    p_Device.UART.Interface = p_Config.UART.Interface;
    p_Device.UART.Rx = p_Config.UART.Rx;
    p_Device.UART.Tx = p_Config.UART.Tx;
    p_Device.UART.Baudrate = p_Config.UART.Baudrate;

    SIM70XX_ERROR_CHECK(SIM70XX_UART_Init(p_Device.UART));

    // Get all remaining available data to clear the Rx buffer.
    SIM70XX_UART_Flush(p_Device.UART);

    // No receive task started yet. Start the receive task.
    SIM70XX_ERROR_CHECK(SIM70XX_Evt_StartTask(&p_Device.Internal.TaskHandle, std::addressof(p_Device)));

    p_Device.Internal.isActive = true;
    p_Device.Internal.isInitialized = true;

    if(Old != SIM_BAUD_AUTO)
    {
        /*
        SIM70XX_Error_t Error;
        ESP_LOGI(TAG, "Changing baudrate...");

        p_Device.Internal.isInitialized = true;
        Error = SIM7080_SetBaudrate(p_Device, Old, p_Config.UART.Baudrate);
        p_Device.Internal.isInitialized = false;
        if(Error != SIM70XX_ERR_OK)
        {
            return Error;
        }*/
    }

	SIM70XX_ERROR_CHECK(SIM7080_SoftReset(p_Device, Timeout));

    vTaskSuspend(p_Device.Internal.TaskHandle);
    SIM70XX_Tools_DisableEcho(p_Device.UART);
    vTaskResume(p_Device.Internal.TaskHandle);

    SIM70XX_ERROR_CHECK(SIM7080_Ping(p_Device));
    SIM70XX_ERROR_CHECK(SIM7080_GetFunctionality(p_Device));

    SIM70XX_ERROR_CHECK(SIM7080_SetFunctionality(p_Device, SIM7080_FUNC_FULL));

    #ifdef CONFIG_SIM70XX_DRIVER_WITH_FS
        SIM70XX_ERROR_CHECK(SIM7080_FS_GetFree(p_Device, &p_Device.FS.Free));
    #endif

    SIM70XX_ERROR_CHECK(SIM7080_SetNetMode(p_Device, p_Config.NetMode));
    SIM70XX_ERROR_CHECK(SIM7080_SetSelectedMode(p_Device, p_Config.Mode));

    if(p_Config.Mode == (SIM7080_MODE_CAT | SIM7080_MODE_NB))
    {
        SIM70XX_ERROR_CHECK(SIM7080_SetBandConfig(p_Device, SIM7080_MODE_CAT, p_Config.Bandlist));
        SIM70XX_ERROR_CHECK(SIM7080_SetBandConfig(p_Device, SIM7080_MODE_NB, p_Config.Bandlist));
    }
    else
    {
        SIM70XX_ERROR_CHECK(SIM7080_SetBandConfig(p_Device, p_Config.Mode, p_Config.Bandlist));
    }

    return SIM70XX_ERR_OK;
}

void SIM7080_Deinit(SIM7080_t& p_Device)
{
    // Stop the receive task.
    vTaskSuspend(p_Device.Internal.TaskHandle);
    vTaskDelete(p_Device.Internal.TaskHandle);
    p_Device.Internal.TaskHandle = NULL;

    // Delete the message queues.
    vQueueDelete(p_Device.Internal.RxQueue);
    vQueueDelete(p_Device.Internal.TxQueue);
    vQueueDelete(p_Device.Internal.EventQueue);

    // TODO: Shutdown modem

    // Deinitialize the modem.
    SIM70XX_UART_Deinit(p_Device.UART);
}

SIM70XX_Error_t SIM7080_SoftReset(SIM7080_t& p_Device, uint32_t Timeout)
{
    uint32_t Now;
    std::string Response;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    if(p_Device.Internal.TaskHandle != NULL)
    {
        vTaskSuspend(p_Device.Internal.TaskHandle);
    }

    ESP_LOGI(TAG, "Performing soft reset...");
    Now = SIM70XX_Tools_GetmsTimer();
    do
    {
        // Reset the module.
        // NOTE: Echo mode is enabled after a reset!
        SIM70XX_UART_SendCommand(p_Device.UART, "ATZ");
        Response = SIM70XX_UART_ReadStringUntil(p_Device.UART);
        Response = SIM70XX_UART_ReadStringUntil(p_Device.UART);
        ESP_LOGI(TAG, "Response: %s", Response.c_str());

        // Check if the reset was successful.
        if(Response.find("OK") != std::string::npos)
        {
            ESP_LOGI(TAG, "     Software reset successful!");

            if(p_Device.Internal.TaskHandle != NULL)
            {
                vTaskResume(p_Device.Internal.TaskHandle);
            }

            return SIM70XX_ERR_OK;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    } while((SIM70XX_Tools_GetmsTimer() - Now) < (Timeout * 1000UL));

    ESP_LOGE(TAG, "     Software reset timeout!");

    if(p_Device.Internal.TaskHandle != NULL)
    {
        vTaskResume(p_Device.Internal.TaskHandle);
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
    Now = SIM70XX_Tools_GetmsTimer();
    do
    {
        SIM70XX_Qual_t Report;

        SIM70XX_ERROR_CHECK(SIM7080_Info_GetQuality(p_Device, &Report));
        if(Report.RSSI != 99)
        {
            isAttached = SIM7080_PGP_GRPS_isAttached(p_Device);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } while((isAttached == false) && ((SIM70XX_Tools_GetmsTimer() - Now) < (Timeout * 1000)));

    if(isAttached == false)
    {
        return SIM70XX_ERR_TIMEOUT;
    }

    SIM70XX_ERROR_CHECK(SIM7080_GetCurrentOperator(p_Device));
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
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

SIM70XX_Error_t SIM7080_GetCurrentOperator(SIM7080_t& p_Device)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_COPS_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
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
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
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
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
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
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
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
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
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
        *p_Band = (SIM7080_Band_t)std::stoi(Response);
    }

    ESP_LOGD(TAG, "Band: %u", *p_Band);

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
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
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
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    *p_Mode = (SIM7080_NetMode_t)std::stoi(Response);

    ESP_LOGD(TAG, "Mode: %u", *p_Mode);

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
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
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
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    *p_Mode = (SIM7080_Mode_t)std::stoi(Response);

    ESP_LOGD(TAG, "Selected mode: %u", *p_Mode);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_SetFunctionality(SIM7080_t& p_Device, SIM7080_Func_t Func, SIM7080_Reset_t Reset)
{
    std::string Status;
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM70XX_Error_t Error = SIM70XX_ERR_FAIL;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device.Connection.Functionality == Func)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT_CFUN_W(Func, Reset);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    // NOTE: Do not use the error macro, because the response and status depends on the current state of the device.
    Error = SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response, &Status);

    // Device is in minimum functionality -> Transition into another functionality. Responses:
    //  OK
    //  +CPIN: READY
    //  SMS Ready
    if(p_Device.Connection.Functionality == SIM7080_FUNC_MIN)
    {
        ESP_LOGI(TAG, "Last: Minimum functionality...");

        if(Response.find("OK") != std::string::npos)
        {
            p_Device.Connection.Functionality = Func;

            Error = SIM70XX_ERR_OK;
        }

        while(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "SMS Ready", &Response) == false)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
    // Device is in full functionality -> Transition into another functionality
    //  Response = +CPIN: NOT READY
    //  Status = OK
    else if(p_Device.Connection.Functionality == SIM7080_FUNC_FULL)
    {
        ESP_LOGI(TAG, "Last: Full functionality...");

        if(Status.find("OK") != std::string::npos)
        {
            p_Device.Connection.Functionality = Func;

            Error = SIM70XX_ERR_OK;
        }
    }

    ESP_LOGI(TAG, "Functionality: %u", p_Device.Connection.Functionality);

    return Error;
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
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    p_Device.Connection.Functionality = (SIM7080_Func_t)std::stoi(Response);

    ESP_LOGI(TAG, "Functionality: %u", p_Device.Connection.Functionality);

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
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
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
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
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