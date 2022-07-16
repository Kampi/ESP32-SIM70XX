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

#include <esp_log.h>

#include "sim7020.h"
#include "../Private/UART/sim70xx_uart.h"
#include "../Private/Events/sim70xx_evt.h"
#include "../Private/Queue/sim70xx_queue.h"
#include "../Private/Commands/sim7020_commands.h"

#ifdef CONFIG_SIM70XX_TASK_CORE_AFFINITY
    #ifndef CONFIG_SIM70XX_TASK_COM_CORE
        #define CONFIG_SIM70XX_TASK_COM_CORE    1
    #endif
#endif

#ifndef CONFIG_SIM70XX_TASK_COM_PRIO
    #define CONFIG_SIM70XX_TASK_COM_PRIO        12
#endif

#ifndef CONFIG_SIM70XX_TASK_COM_STACK
    #define CONFIG_SIM70XX_TASK_COM_STACK       4096
#endif

static const char* TAG = "SIM7020";

SIM70XX_Error_t SIM7020_Init(SIM7020_t* const p_Device, const SIM7020_Config_t* const p_Config, uint32_t Timeout)
{
    return SIM7020_Init(p_Device, p_Config, Timeout, SIM_BAUD_AUTO);
}

SIM70XX_Error_t SIM7020_Init(SIM7020_t* const p_Device, const SIM7020_Config_t* const p_Config, SIM70XX_Baud_t Old)
{
    return SIM7020_Init(p_Device, p_Config, 10, Old);
}

SIM70XX_Error_t SIM7020_Init(SIM7020_t* const p_Device, const SIM7020_Config_t* const p_Config, uint32_t Timeout, SIM70XX_Baud_t Old)
{
    std::string Response;

    if((p_Config == NULL) || (p_Device == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized)
    {
        SIM7020_Deinit(p_Device);
    }

    p_Device->Internal.RxQueue = xQueueCreate(CONFIG_SIM70XX_QUEUE_LENGTH, sizeof(SIM70XX_CmdResp_t*));
    p_Device->Internal.TxQueue = xQueueCreate(CONFIG_SIM70XX_QUEUE_LENGTH, sizeof(SIM70XX_TxCmd_t*));
    p_Device->Internal.EventQueue = xQueueCreate(CONFIG_SIM70XX_QUEUE_LENGTH, sizeof(std::string*));
    if((p_Device->Internal.RxQueue == NULL) || (p_Device->Internal.TxQueue == NULL) || (p_Device->Internal.EventQueue == NULL))
    {
        return SIM70XX_ERR_NO_MEM;
    }

    p_Device->UART.Interface = p_Config->UART.Interface;
    p_Device->UART.Rx = p_Config->UART.Rx;
    p_Device->UART.Tx = p_Config->UART.Tx;
    p_Device->UART.Baudrate = p_Config->UART.Baudrate;

    SIM70XX_ERROR_CHECK(SIM70XX_UART_Init(&p_Device->UART));

    // Get all remaining available data to clear the Rx buffer.
    SIM70XX_UART_Flush(&p_Device->UART);

    // No receive task started yet. Start the receive task.
    SIM70XX_ERROR_CHECK(SIM70XX_Evt_StartTask(&p_Device->Internal.TaskHandle, p_Device));

    p_Device->Internal.isActive = true;
    p_Device->Internal.isInitialized = true;

    if(Old != SIM_BAUD_AUTO)
    {
        SIM70XX_Error_t Error;
        ESP_LOGI(TAG, "Changing baudrate...");

        p_Device->Internal.isInitialized = true;
        Error = SIM7020_SetBaudrate(p_Device, Old, p_Config->UART.Baudrate);
        p_Device->Internal.isInitialized = false;
        if(Error != SIM70XX_ERR_OK)
        {
            return Error;
        }
    }

	#ifdef CONFIG_SIM70XX_RESET_USE_HW
        p_Device->Interface.Reset_Conf.Inverted = p_Config->Interface.Reset_Conf.Inverted;
        p_Device->Interface.Reset_Conf.Pin = p_Config->Interface.Reset_Conf.Pin;
		if(p_Device->Interface.Reset_Conf.Pin != GPIO_NUM_NC)
		{
		    gpio_set_direction(p_Device->Interface.Reset_Conf.Pin, GPIO_MODE_OUTPUT);
		    SIM7020_HardReset(p_Device);
		}
    #else
        SIM70XX_ERROR_CHECK(SIM7020_SoftReset(p_Device, Timeout));
	#endif

    vTaskSuspend(p_Device->Internal.TaskHandle);
    SIM70XX_DisableEcho(&p_Device->UART);
    vTaskResume(p_Device->Internal.TaskHandle);

    SIM70XX_ERROR_CHECK(SIM7020_Ping(p_Device));
    SIM70XX_ERROR_CHECK(SIM7020_PSM_Disable(p_Device, SIM7020_PSM_DIS));
    if(SIM7020_GetFunctionality(p_Device) == SIM70XX_ERR_INVALID_STATE)
    {

    }
    SIM70XX_ERROR_CHECK(SIM7020_GetBand(p_Device));
    SIM70XX_ERROR_CHECK(SIM7020_SetPSD(p_Device, SIM7020_PDP_IP, p_Config->APN));
    SIM70XX_ERROR_CHECK(SIM7020_SetBand(p_Device, p_Config->Band));
    SIM70XX_ERROR_CHECK(SIM7020_SetOperator(p_Device, SIM7020_MODE_MANUAL, p_Config->OperatorFormat, p_Config->Operator));
    SIM70XX_ERROR_CHECK(SIM7020_Info_GetNetworkRegistrationStatus(p_Device));
    SIM70XX_ERROR_CHECK(SIM7020_Info_GetQuality(p_Device));
    //SIM70XX_ERROR_CHECK(SIM7020_PDP_ReadDynamicParameters(p_Device));
    //SIM70XX_ERROR_CHECK(SIM7020_PSM_GetEventStatus(p_Device, &p_Device->Internal.isPSMEvent));

/*
    SIM7020_PSM_Enable(p_Device, 3, 15, 0, 15);
    SIM70XX_ERROR_CHECK(SIM7020_PSM_SetEventStatus(p_Device, true));
    Error = SIM7020_GetOperator(p_Device, &p_Device->Operators, &p_Device->Modes, &p_Device->Formats);
*/

    return SIM70XX_ERR_OK;
}

void SIM7020_Deinit(SIM7020_t* const p_Device)
{
    assert(p_Device);

    // Stop the receive task.
    vTaskSuspend(p_Device->Internal.TaskHandle);
    vTaskDelete(p_Device->Internal.TaskHandle);
    p_Device->Internal.TaskHandle = NULL;

    // Delete the queues.
    vQueueDelete(p_Device->Internal.RxQueue);
    vQueueDelete(p_Device->Internal.TxQueue);
    vQueueDelete(p_Device->Internal.EventQueue);

    // TODO: Shutdown modem

    // Deinitialize the modem.
    SIM70XX_UART_Deinit(&p_Device->UART);
}

#ifdef CONFIG_SIM70XX_RESET_USE_HW
	void SIM7020_HardReset(SIM7020_t* const p_Device )
	{
		assert(p_Device);

		if(p_Device->Interface.Reset_Conf.Pin != GPIO_NUM_NC)
		{
			if(p_Device->Interface.Reset_Conf.Inverted)
			{
				gpio_set_level(p_Device->Interface.Reset_Conf.Pin, true);
				vTaskDelay(1000 / portTICK_PERIOD_MS);
				gpio_set_level(p_Device->Interface.Reset_Conf.Pin, false);
			}
		}
	}
#endif

SIM70XX_Error_t SIM7020_SoftReset(const SIM7020_t* const p_Device, uint32_t Timeout)
{
    uint32_t Now;
    std::string Response;

    if(p_Device == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    if(p_Device->Internal.TaskHandle != NULL)
    {
        vTaskSuspend(p_Device->Internal.TaskHandle);
    }

    ESP_LOGI(TAG, "Performing soft reset...");
    Now = SIM70XX_Tools_GetmsTimer();
    do
    {
        // Reset the module.
        // NOTE: Echo mode is enabled after a reset!
        SIM70XX_UART_SendCommand(&p_Device->UART, "ATZ");
        Response = SIM70XX_UART_ReadStringUntil(&p_Device->UART, '\n');
        Response = SIM70XX_UART_ReadStringUntil(&p_Device->UART, '\n');
        ESP_LOGI(TAG, "Response: %s", Response.c_str());

        // Check if the reset was successful.
        if(Response.find("OK") != std::string::npos)
        {
            ESP_LOGI(TAG, "     Software reset successful!");

            if(p_Device->Internal.TaskHandle != NULL)
            {
                vTaskResume(p_Device->Internal.TaskHandle);
            }

            return SIM70XX_ERR_OK;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    } while((SIM70XX_Tools_GetmsTimer() - Now) < (Timeout * 1000UL));

    ESP_LOGE(TAG, "     Software reset timeout!");

    if(p_Device->Internal.TaskHandle != NULL)
    {
        vTaskResume(p_Device->Internal.TaskHandle);
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7020_SetPSD(SIM7020_t* const p_Device, SIM7020_PDP_Type_t PDP, SIM7020_APN_t APN)
{
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;
    SIM70XX_Error_t Error;

    if(p_Device == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
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

    if((APN.Username.length() > 0) && (APN.Password.length() > 0))
    {
        CommandStr += ",\"" + APN.Username + "\",\"" + APN.Password + "\"";
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_MCGDEFCONT(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    // NOTE: We can not use the error macro here because Response and Status are swapped.
    Error = SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, NULL, &CommandStr);
    if(CommandStr.find("OK") == std::string::npos)
    {
        return Error;
    }

    SIM70XX_ERROR_CHECK(SIM7020_SetFunctionality(p_Device, SIM7020_FUNC_FULL));

    p_Device->PDP_Type = PDP;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_SetOperator(SIM7020_t* const p_Device, SIM7020_OpMode_t Mode, SIM7020_OpForm_t Format, std::string Operator)
{
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if(p_Device == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    CommandStr = "AT+COPS=" + std::to_string(Mode);

    if((Mode == SIM7020_MODE_MANUAL) || (Mode == SIM7020_MODE_BOTH))
    {
        CommandStr += "," + std::to_string(Format) + ",\"" + Operator + "\"";
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_COPS_W(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, NULL, NULL);
}

SIM70XX_Error_t SIM7020_GetOperator(SIM7020_t* const p_Device, std::vector<SIM7020_Operator_t>* p_Operator, std::string* p_Modes, std::string* p_Formats)
{
    int End;
    int Start;
    std::string OperatorList;

    if((p_Device == NULL) || (p_Operator == NULL) || (p_Modes == NULL) || (p_Formats == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    p_Operator->clear();

    // The device must be active to check the operators.
    /*
    Error = SIM7020_SetFunctionality(p_Device, SIM7020_FUNC_FULL);
    if(Error)
    {
        return Error;
    }

    _TxCommand = SIM7020_AT_COPS_R;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, _TxCommand);

    //SIM70XX_ERROR_CHECK(SIM7020_Wait(p_Device, &_RxCommand));

    // Get the modes list.
    Start = _RxCommand.Response.indexOf(",,") + 3;
    End = _RxCommand.Response.indexOf("),(", Start);
    *p_Modes = _RxCommand.Response.substring(Start, End);

    // Get the formats list.
    *p_Formats = _RxCommand.Response.substring(End + 3, _RxCommand.Response.length() - 1);

    // Get the operators list.
    _RxCommand.Response.remove(Start, _RxCommand.Response.length() - Start);
    OperatorList = _RxCommand.Response.substring(_RxCommand.Response.indexOf("("), _RxCommand.Response.lastIndexOf(")") + 1);

    ESP_LOGD(TAG, "Operator list: %s", OperatorList.c_str());

    // Get each operator.
    String Dummy;
    do
    {
        int Start;

        Start = OperatorList.indexOf("(");

        Dummy = OperatorList.substring(Start, OperatorList.indexOf(")", Start));
        OperatorList.replace(Dummy, "");
        Dummy.replace("(", "");
        Dummy.replace(")", "");

        if(Dummy.length() > 0)
        {
            int End;
            int Index = 0;
            SIM7020_Operator_t Operator;

            Index = 0;
            End = Dummy.indexOf(",", Index);
            Operator.Stat = (SIM7020_OpStat_t)Dummy.substring(Index, End).toInt();

            Index = End + 1;
            End = Dummy.indexOf(",", Index);
            Operator.Long = Dummy.substring(Index, End);

            Index = End + 1;
            End = Dummy.indexOf(",", Index);
            Operator.Short = Dummy.substring(Index, End);

            Index = End + 1;
            End = Dummy.indexOf(",", Index);
            Operator.Numeric = Dummy.substring(Index, End);

            Index = End + 1;
            End = Dummy.indexOf(",", Index);
            Operator.Act = Dummy.substring(Index, End).toInt();

            p_Operator->push_back(Operator);
        }
    } while(Dummy.length() > 0);

    ESP_LOGD(TAG, "Found %u operators...", p_Operator->size());

    for(std::list<SIM7020_Operator_t>::const_iterator it = p_Operator->begin(); it != p_Operator->end(); ++it)
    {
        ESP_LOGD(TAG, "     Stat: %u", it->Stat);
        ESP_LOGD(TAG, "     Long: %s", it->Long.c_str());
        ESP_LOGD(TAG, "     Short: %s", it->Short.c_str());
        ESP_LOGD(TAG, "     Numeric: %s", it->Numeric.c_str());
        ESP_LOGD(TAG, "     Stat: %u", it->Act);
    }

    ESP_LOGD(TAG, "Modes: %s", p_Modes->c_str());
    ESP_LOGD(TAG, "Formats: %s", p_Formats->c_str());
*/
    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_SetBand(SIM7020_t* const p_Device, SIM7020_Band_t Band)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device->Band == Band)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CBAND_W(Band);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue));

    p_Device->Band = Band;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_GetBand(SIM7020_t* const p_Device)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Device == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device->Connection.Functionality == SIM7020_FUNC_MIN)
    {
        return SIM70XX_ERR_INVALID_STATE;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CBAND_R;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    p_Device->Band = (SIM7020_Band_t)std::stoi(Response);

    ESP_LOGD(TAG, "Band: %u", p_Device->Band);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_SetFunctionality(SIM7020_t* const p_Device, SIM7020_Func_t Func)
{
    std::string Status;
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM70XX_Error_t Error = SIM70XX_ERR_FAIL;

    if(p_Device == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device->Connection.Functionality == Func)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CFUN_W(Func);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    // NOTE: Do not use the error macro, because the response and status depends on the current state of the device.
    Error = SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response, &Status);

    // Device is in minimum functionality -> Transition into another functionality
    //  Response = OK
    //  Status = +CPIN: READY
    if(p_Device->Connection.Functionality == SIM7020_FUNC_MIN)
    {
        ESP_LOGI(TAG, "Minimum functionality...");

        if(Response.find("OK") != std::string::npos)
        {
            p_Device->Connection.Functionality = Func;

            Error = SIM70XX_ERR_OK;
        }
    }
    // Device is in full functionality -> Transition into another functionality
    //  Response = +CPIN: NOT READY
    //  Status = OK
    else if(p_Device->Connection.Functionality == SIM7020_FUNC_FULL)
    {
        ESP_LOGI(TAG, "Full functionality...");

        if(Status.find("OK") != std::string::npos)
        {
            p_Device->Connection.Functionality = Func;

            Error = SIM70XX_ERR_OK;
        }
    }

    ESP_LOGI(TAG, "Set functionality: %u", p_Device->Connection.Functionality);

    return Error;
}

SIM70XX_Error_t SIM7020_GetFunctionality(SIM7020_t* const p_Device)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Device == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CFUN_R;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    p_Device->Connection.Functionality = (SIM7020_Func_t)std::stoi(Response);

    ESP_LOGD(TAG, "Functionality: %u", p_Device->Connection.Functionality);

    return SIM70XX_ERR_OK;
}

bool SIM7020_isInitialized(SIM7020_t* const p_Device)
{
    if(p_Device == NULL)
    {
        return false;
    }

    return p_Device->Internal.isInitialized;
}

bool SIM7020_isPSM(SIM7020_t* const p_Device)
{
    if(p_Device == NULL)
    {
        return false;
    }

    return p_Device->Internal.isPSM;
}

SIM70XX_Error_t SIM7020_Ping(SIM7020_t* const p_Device)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }

    if(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue) != SIM70XX_ERR_OK)
    {
        return SIM70XX_ERR_NOT_READY;
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_SetBaudrate(SIM7020_t* const p_Device, SIM70XX_Baud_t Old, SIM70XX_Baud_t New)
{
    std::string Status;
    SIM70XX_TxCmd_t* Command;

    if(p_Device == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device->UART.Baudrate == New)
    {
        return SIM70XX_ERR_OK;
    }

    // Initialize the serial interface with the old baudrate.
    vTaskSuspend(p_Device->Internal.TaskHandle);
    p_Device->UART.Baudrate = Old;
    SIM70XX_ERROR_CHECK(SIM70XX_UART_Deinit(&p_Device->UART));
    SIM70XX_ERROR_CHECK(SIM70XX_UART_Init(&p_Device->UART));
    vTaskResume(p_Device->Internal.TaskHandle);

    // Set the new baudrate.
    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_IPR_W(New);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Status));

    if(Status.find("OK") == std::string::npos)
    {
        ESP_LOGE(TAG, "Can not enable new baudrate!");

        // Switch back to the old baudrate.
        vTaskSuspend(p_Device->Internal.TaskHandle);
        p_Device->UART.Baudrate = Old;
        SIM70XX_ERROR_CHECK(SIM70XX_UART_Deinit(&p_Device->UART));
        SIM70XX_ERROR_CHECK(SIM70XX_UART_Init(&p_Device->UART));
        vTaskResume(p_Device->Internal.TaskHandle);

        return SIM70XX_ERR_FAIL;
    }

    ESP_LOGI(TAG, "New baudrate enabled. Reinitialize the interface!");

    // Reinitialize the interface with the new baudrate.
    vTaskSuspend(p_Device->Internal.TaskHandle);
    p_Device->UART.Baudrate = New;
    SIM70XX_ERROR_CHECK(SIM70XX_UART_Deinit(&p_Device->UART));
    SIM70XX_ERROR_CHECK(SIM70XX_UART_Init(&p_Device->UART));
    vTaskResume(p_Device->Internal.TaskHandle);

    return SIM70XX_ERR_OK;
}

#endif